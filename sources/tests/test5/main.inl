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

static void move(vec4 &pos, const vec4 &vel, float dt)
{
  pos.x += vel.x * dt;
  pos.y += vel.y * dt;
  pos.z += vel.z * dt;
  pos.w += vel.w * dt;
}

struct IMovable
{
  virtual void move(float dt)
  {
  }
};
struct GameObject : public IMovable
{
  ecs::string name;
  ecs::string text;
  vec4 vel;
  mat4 data[5];
  vec4 pos;
  void move(float dt) override
  {
    ::move(pos, vel, dt);
  }
};

ECS_TYPE_REGISTRATION(int, "int", true, true, true, {})
ECS_TYPE_REGISTRATION(vec4, "vec4", true, true, true, {})
ECS_TYPE_REGISTRATION(mat4, "mat4", true, true, true, {})
ECS_TYPE_REGISTRATION(ecs::string, "string", false, true, false, {})

static ecs::vector<GameObject> go_array;
static ecs::vector<std::shared_ptr<GameObject>> go_ptr_array;
static ecs::vector<IMovable*> go_i_ptr_array;

static ecs::vector<vec4> soa_pos;
static ecs::vector<vec4> soa_vel;

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
    TimeProfile a("go pointers array creation");
    for (int i = 0; i < N; i++)
    {
      GameObject &go = *(go_ptr_array.emplace_back(std::make_shared<GameObject>()));
      go.name = "i)";
      float x = i * 0.01f;
      go.pos = vec4{x, x, x, x};
      go.vel = vec4{x, x, x, x};
    }
  }
  {
    TimeProfile a("go pointers array creation");
    for (int i = 0; i < N; i++)
    {
      auto ptr = new GameObject();
      go_i_ptr_array.emplace_back(ptr);
      GameObject &go = *ptr;
      go.name = "i)";
      float x = i * 0.01f;
      go.pos = vec4{x, x, x, x};
      go.vel = vec4{x, x, x, x};
    }
  }
  {
    TimeProfile a("entity creation");
    for (int i = 0; i < N; i++)
    {
      float x = i * 0.01f;
      ecs::create_entity_immediate(p, {{"pos", vec4{x, x, x, x}}, {"vel", vec4{x, x, x, x}}});
    }
  }

#define TEST_PERF(func)           \
  {                               \
    TimeProfile a(#func);         \
    void func##_implementation(); \
    func##_implementation();      \
  }
  TEST_PERF(array_iteration)
  TEST_PERF(array_ptr_iteration)
  TEST_PERF(array_i_ptr_iteration)
  TEST_PERF(ecs_system_iteration)
  TEST_PERF(ecs_query_iteration)

  ecs::destroy_all_entities();
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
array_i_ptr_iteration()
{
  for (auto &go : go_i_ptr_array)
  {
    go->move(dt);
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
