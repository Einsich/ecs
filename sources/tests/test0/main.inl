#include <ecs/ecs.h>
#include "../tests.h"

ECS_EVENT_REGISTRATION(MyEvent, "MyEvent", true)
ECS_REQUEST_REGISTRATION(MyRequest, "MyRequest")

int main()
{
  ecs::init();
  ecs::pull_registered_files();

  ecs::prefab_id p = ecs::create_entity_prefab({"lol", {{"x", 1.f}, {"y", 2.f}, {"z", 3}}});

  auto eid1 = ecs::create_entity_immediate(p);
  ecs::create_entity_immediate(p, {{"x", 3.f}, {"z", -2.f}});
  ecs::create_entity_immediate(p, {{"x", 2.f}, {"z", -1.f}});
  ecs::create_entity_immediate(p, {{"x", 1.f}, {"z", -0.f}});

  ecs::update_query_manager(); // after register_event, register_request, etc
  ecs::perform_systems();

  ecs::destroy_entity(eid1);
  ecs::destroy_entity(eid1);
  ecs::update_archetype_manager();
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
  return 0;
}

template <typename C>
static void test_single_query(ecs::EntityId eid, C c);

SYSTEM()
test(const float &x, const float &y, const int &z, ecs::EntityId eid)
{
  printf("hi %f %f %d\n", x, y, z);
  print(eid);

  QUERY()
  test_single_query(eid, [&](int &z, float &y)
                    { y+=10; z-=10; });

  printf("bye %f %f %d\n", x, y, z);
}

EVENT()
test_event(const MyEvent &e, float y)
{
  printf("test_event %d %f\n", e.x, y);
}

REQUEST(require = float x)
test_request(MyRequest &r)
{
  r.count += 1;
}
