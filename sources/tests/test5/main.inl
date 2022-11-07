#include <ecs/ecs.h>
#include <ecs/time_profiler.h>
#include <ecs/type_registration.h>

struct vec4
{
  float x, y, z, w;
};

struct mat4
{
  vec4 row0, row1, row2, row3;
};

struct GameObject
{
  ecs::string name;
  ecs::string text;
  vec4 vel;
  mat4 data[5];
  vec4 pos;
};

ECS_TYPE_REGISTRATION(vec4, "vec4", true, true, true, {})
ECS_TYPE_REGISTRATION(mat4, "mat4", true, true, true, {})
ECS_TYPE_REGISTRATION(ecs::string, "string", false, true, false, {})

static ecs::vector<GameObject> go_array;

int main()
{
  ecs::init();
  ecs::pull_registered_files();

  ecs::prefab_id p = ecs::create_entity_prefab(
      {"go",
       {
           /*
              {"name", ecs::string()},
              {"text", ecs::string()},
              {"vel", vec4()},
              {"data0", mat4()},
              {"data1", mat4()},
              {"data2", mat4()},
              {"data3", mat4()},
              {"data4", mat4()}, */
           //{"pos", vec4()},
       },
       ecs::SizePolicy::Thousands});

  int N = 30000;
  {
    TimeProfile a("go array creation");
    for (int i = 0; i < N; i++)
    {
      GameObject &go = go_array.emplace_back();
      go.name = "i)";
      float x = i * 0.01f;
      go.pos = vec4{x, x, x, x};
      go.vel = vec4{x, x, x, x};
    }
  }
  {
    int k = 4094 * 2;
    for (int i = 0; i < 60; i++)
      new char[k];
    TimeProfile a("entity creation");
    for (int i = 0; i < N; i++)
    {
      float x = i * 0.01f;
      // {{"pos", vec4{x, x, x, x}}, {"vel", vec4{x, x, x, x}}};
      ecs::create_entity_immediate(p);
    }
  }

#define TEST_PERF(func)           \
  {                               \
    TimeProfile a(#func);         \
    void func##_implementation(); \
    func##_implementation();      \
  }
  TEST_PERF(array_iteration)
  TEST_PERF(ecs_system_iteration)
  TEST_PERF(ecs_query_iteration)

  ecs::destroy_all_entities();
  return 0;
}

static void move(vec4 &pos, const vec4 &vel, float dt)
{
  pos.x += vel.x * dt;
  pos.y += vel.y * dt;
  pos.z += vel.z * dt;
  pos.w += vel.w * dt;
}

float dt = 0.2f;

SYSTEM()
array_iteration()
{
  TimeProfile a("copy constuctor");

  for (GameObject &go : go_array)
  {
    move(go.pos, go.vel, dt);
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
