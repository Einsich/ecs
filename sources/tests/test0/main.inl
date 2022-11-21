#include <ecs/ecs.h>
#include "../tests.h"

ECS_EVENT_REGISTRATION(MyEvent, "MyEvent", true)
ECS_REQUEST_REGISTRATION(MyRequest, "MyRequest")

int main()
{
  ecs::init();
  ecs::pull_registered_files();

  ecs::prefab_id p = ecs::create_entity_prefab({"lol", {{"x", 1}, {"y", 2}, {"z", 3}}});

  auto eid1 = ecs::create_entity_immediate(p);
  ecs::create_entity_immediate(p, {{"x", 3}, {"z", -2.f}});
  ecs::create_entity_immediate(p, {{"x", 2}, {"z", -1.f}});
  ecs::create_entity_immediate(p, {{"x", 1}, {"z", -0.f}});

  ecs::update_query_manager(); // after register_event, register_request, etc
  printf("ecs::perform_systems()\n\n");
  ecs::perform_systems();

  ecs::destroy_entity(eid1);
  ecs::destroy_entity(eid1);
  ecs::update_archetype_manager();
  printf("ecs::perform_systems()\n\n");
  ecs::perform_systems();

  MyEvent m;
  m.x = 69;
  ecs::send_event_immediate(m);
  m.x = 96;
  ecs::send_event(m);

  MyRequest r;
  ecs::send_request(r);
  printf("request count %d\n", r.count);

  ecs::destroy_all_entities();
  std::fflush(stdout);
  return 0;
}

template <typename C>
static void test_single_query(ecs::EntityId eid, C c);

SYSTEM()
test(const int &x, const int &y, const int &z, ecs::EntityId eid)
{
  printf("hi %d %d %d\n", x, y, z);
  print(eid);

  QUERY()
  test_single_query(eid, [&](int &z, int &y)
                    { y+=10; z-=10; });

  printf("bye %d %d %d\n\n", x, y, z);
}

EVENT()
test_event(const MyEvent &e, int y)
{
  printf("test_event %d %d\n\n", e.x, y);
}

REQUEST(require = int x)
test_request(MyRequest &r)
{
  r.count += 1;
}
