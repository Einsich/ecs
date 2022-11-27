#include <ecs/ecs.h>
#include <ecs/time_profiler.h>
#include <ecs/type_registration.h>

#include <iostream>
#include <map>

#include "my_time.h"

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

static void move(vec4 &pos, const vec4 &vel, float dt)
{
  pos.x += vel.x * dt;
  pos.y += vel.y * dt;
  pos.z += vel.z * dt;
  pos.w += vel.w * dt;
}

ECS_TYPE_REGISTRATION(float, "float", true, true, true)
ECS_TYPE_REGISTRATION(vec4, "vec4", true, true, true)

static int perf_test(const char *label, void (*test)())
{
  trash_cache();
  TimeProfile a(label);
  test();
  return a.get_time();
}

int main()
{
  ecs::init();
  ecs::init_singletones();
  ecs::pull_registered_files();

  ecs::prefab_id p = ecs::create_entity_prefab(
      {"go",
       {
           {"vel", vec4()},
           {"pos", vec4()},
           {"dt", 0.25f},
       },
       ecs::SizePolicy::Thousands});

  int N = 3000;
  {
    TimeProfile a("entity creation");
    for (int i = 0; i < N; i++)
    {
      float x = i * 0.01f;
      ecs::create_entity_immediate(p, {{"pos", vec4{x, x, x, x}}, {"vel", vec4{x, x, x, x}}});
    }
  }

  struct Test
  {
    ecs::string name;
    std::function<int()> benchmark;
    int sumOfTime, maxTime;
  };

#define TESTS                               \
  TEST(ecs_system_iteration)                \
  TEST(ecs_query_iteration)                 \
  TEST(ecs_system_with_singleton_iteration) \
  TEST(ecs_query_with_singleton_iteration)

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
  int n = 4 * 3 * 2;

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

  for (const Test &test : tests)
  {
    ECS_LOG("spent avg %d, max %d us. in %s\n", test.sumOfTime / n, test.maxTime, test.name.c_str());
  }
  std::cout << std::endl;
  ecs::destroy_all_entities();
  ecs::destroy_sinletons();
  std::fflush(stdout);
  return 0;
}

float dt = 0.2f;

ECS_REGISTER_SINGLETON(GlobalTime)

SYSTEM()
ecs_system_iteration(vec4 &pos, const vec4 &vel)
{
  move(pos, vel, get_time().dt);
}

SYSTEM()
ecs_system_with_singleton_iteration(vec4 &pos, const vec4 &vel, const GlobalTime &globalTime)
{
  move(pos, vel, globalTime.dt);
}
template <typename C>
static void query(C);

SYSTEM()
ecs_query_iteration()
{
  QUERY()
  query([&](vec4 &pos, const vec4 &vel)
        { move(pos, vel, get_time().dt); });
}

template <typename C>
static void query_with_singleton(C);

SYSTEM()
ecs_query_with_singleton_iteration()
{
  QUERY()
  query_with_singleton([](vec4 &pos, const vec4 &vel, const GlobalTime &globalTime)
                       { move(pos, vel, globalTime.dt); });
}