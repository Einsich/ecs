#include <ecs/ecs.h>
#include <ecs/time_profiler.h>
#include <ecs/type_registration.h>

#include <iostream>
#include <map>
static void trash_cache()
{
  static std::map<int, int> bin;

  for (int i = 0; i < 100000; i++)
  {
    bin[rand()] = i;
  }
  bin.clear();
}

struct vec4
{
  float x, y, z, w;
};

struct mat4
{
  vec4 row0, row1, row2, row3;
};

static void move(vec4 &pos, const vec4 &vel, float dt)
{
  pos.x += vel.x * dt;
  pos.y += vel.y * dt;
  pos.z += vel.z * dt;
  pos.w += vel.w * dt;
}

struct GameObject
{
  // ecs::string name;
  // ecs::string text;
  mat4 data1[20];
  vec4 vel;
  mat4 data2[20];
  vec4 pos;
};

ECS_TYPE_REGISTRATION(vec4, "vec4", ecs::PODType)
ECS_TYPE_REGISTRATION(mat4, "mat4", ecs::PODType)
ECS_TYPE_REGISTRATION(ecs::string, "string", ecs::TrivialMoveConstructor)

static int perf_test(const char *label, void (*test)())
{
  trash_cache();
  TimeProfile a(label);
  test();
  return a.get_time();
}

static ecs::vector<GameObject> go_array;
static ecs::vector<GameObject *> go_ptr_array;

static ecs::vector<vec4> soa_pos;
static ecs::vector<vec4> soa_vel;

template <typename T>
static void shuffle(ecs::vector<T> &v)
{
  for (int i = 0, n = v.size(); i < n; i++)
  {
    std::swap(v[i], v[rand() % n]);
  }
}

int main()
{
  ecs::init();
  ecs::pull_registered_files();
  // ecs::prefab_id p1 = ecs::create_entity_prefab({"hi", {{"i", 1}}});
  // ecs::prefab_id p2 = ecs::create_entity_prefab({"ho", {{"j", 1}}});
  // ecs::prefab_id p3 = ecs::create_entity_prefab({"ha", {{"k", 1}}});

  ecs::prefab_id p = ecs::create_entity_prefab(
      {"go",
       {
           {"name", ecs::string()},
           {"text", ecs::string()},
           {"vel", vec4()},
           {"data0", mat4()},
           {"data1", mat4()},
           {"data2", mat4()},
           {"data3", mat4()},
           {"data4", mat4()},
           {"pos", vec4()},
       },
       ecs::SizePolicy::Thousands});

  int N = 100000;
  {
    TimeProfile a("go array creation");
    for (int i = 0; i < N; i++)
    {
      GameObject &go = go_array.emplace_back();
      // go.name = "i)";
      float x = i * 0.01f;
      go.pos = vec4{x, x, x, x};
      go.vel = vec4{x, x, x, x};
    }
  }
  {
    TimeProfile a("go pointers array creation");
    for (int i = 0; i < N; i++)
    {
      GameObject &go = *go_ptr_array.emplace_back(new GameObject());
      // go.name = "i)";
      float x = i * 0.01f;
      go.pos = vec4{x, x, x, x};
      go.vel = vec4{x, x, x, x};
    }
    shuffle(go_ptr_array);
  }
  {
    TimeProfile a("entity creation");
    for (int i = 0; i < N; i++)
    {
      float x = i * 0.01f;
      ecs::create_entity_immediate(p, {{"pos", vec4{x, x, x, x}}, {"vel", vec4{x, x, x, x}}});
    }
  }
  {
    TimeProfile a("soa creation");
    for (int i = 0; i < N; i++)
    {
      float x = i * 0.01f;
      soa_pos.push_back(vec4{x, x, x, x});
      soa_vel.push_back(vec4{x, x, x, x});
    }
  }

  struct Test
  {
    ecs::string name;
    std::function<int()> benchmark;
    int sumOfTime, maxTime;
  };

#define TESTS                \
  TEST(array_iteration)      \
  TEST(array_ptr_iteration)  \
  TEST(ecs_system_iteration) \
  TEST(soa_iteration)

#define TEST(test) void test##_implementation();
  TESTS
#undef TEST

#define TEST(test) {#test, []() { return perf_test(#test, test##_implementation); }, 0, 0},

  ecs::vector<Test> tests = {
      TESTS};
#undef TEST

  ecs::vector<int> order;
  int k = tests.size();
  for (int i = 0; i < k; i++)
  {
    order.push_back(i);
  }
  int n = 4 * 3 * 2 * 20;

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < k; j++)
    {
      Test &test = tests[order[j]];
      int t = test.benchmark();
      test.sumOfTime += t;
      test.maxTime = test.maxTime < t ? t : test.maxTime;
    }
    std::next_permutation(order.begin(), order.end());
  }
  printf("sizeof %llu\n", sizeof(GameObject));
  for (const Test &test : tests)
  {
    ECS_LOG("spent avg %d, max %d us. in %s\n", test.sumOfTime / n, test.maxTime, test.name.c_str());
  }
  std::cout << std::endl;
  ecs::destroy_all_entities();
  std::fflush(stdout);
  return 0;
}

float dt = 0.2f;

SYSTEM()
array_iteration()
{
  for (GameObject &go : go_array)
  {
    move(go.pos, go.vel, dt);
  }
}

SYSTEM()
array_ptr_iteration()
{
  for (auto &go : go_ptr_array)
  {
    move(go->pos, go->vel, dt);
  }
}

SYSTEM()
ecs_system_iteration(vec4 &pos, const vec4 &vel)
{
  move(pos, vel, dt);
}

template <typename C>
static void query(C);

SYSTEM()
ecs_query_iteration()
{
  QUERY()
  query([](vec4 &pos, const vec4 &vel)
        { move(pos, vel, dt); });
}

SYSTEM()
soa_iteration()
{
  int n = soa_pos.size();
  vec4 *pos = soa_pos.data();
  const vec4 *vel = soa_vel.data();
  for (int i = 0; i < n; ++i, ++pos, ++vel)
  {
    move(*pos, *vel, dt);
  }
}
