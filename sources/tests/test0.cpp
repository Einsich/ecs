#include <ecs/a.h>
#include <ecs/ecs.h>

ecs::QueryCache cache;
static void test_system()
{
  printf("hi\n");
}

int main()
{
  ecs::register_system(ecs::SystemDescription(
      "test0.cpp",
      "test_system",
      &cache,
      {}, // args
      {}, // req
      {}, // req_not
      {}, // before
      {}, // after
      {}, // tags
      &test_system));

  ecs::EntityPrefab p("lol", {{"x", 1.f}, {"y", 2.f}, {"z", 3}});

  ecs::create_entity_immediate(p);

  ecs::perform_systems();
  f();
  return 0;
}