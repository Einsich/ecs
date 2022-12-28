#include <ecs/ecs.h>
#include <ecs/event_registration.h>
#include <ecs/request_registration.h>
#include "../tests.h"

ECS_EVENT_REGISTRATION(MyEvent)
ECS_REQUEST_REGISTRATION(MyRequest)

int main()
{
  ecs::init();
  ecs::pull_registered_files();

  ecs::prefab_id p = ecs::create_entity_prefab({"lol", {{"x", 1}, {"y", 2}, {"z", 3}}});

  auto eid1 = ecs::create_entity_immediate(p);
  ecs::create_entity_immediate(p, {{"x", 3}, {"z", -2.f}});
  ecs::create_entity_immediate(p, {{"x", 2}, {"z", -1.f}});
  ecs::create_entity_immediate(p, {{"x", 1}, {"z", -0.f}});

  ecs::perform_deffered_events(); // after register_event, register_request, etc
  ECS_LOG("ecs::perform_stage()\n");
  ecs::perform_stage("");

  ecs::destroy_entity(eid1);
  ecs::destroy_entity(eid1);
  ecs::update_archetype_manager();
  ECS_LOG("ecs::perform_stage()\n");
  ecs::perform_stage("");

  MyEvent m;
  m.x = 69;
  ecs::send_event_immediate(m);
  m.x = 96;
  ecs::send_event(m);

  MyRequest r;
  ecs::send_request(r);
  ECS_LOG("request count %d", r.count);

  ecs::destroy_all_entities();
  std::fflush(stdout);
  return 0;
}

template <typename C>
static void test_single_query(ecs::EntityId eid, C c);

SYSTEM()
test(const int &x, const int &y, const int &z, ecs::EntityId eid)
{
  ECS_LOG("hi %d %d %d", x, y, z);
  print(eid);

  QUERY()
  test_single_query(eid, [&](int &z, int &y)
                    { y+=10; z-=10; });

  ECS_LOG("bye %d %d %d\n", x, y, z);
}

EVENT()
test_event(const MyEvent &e, int y)
{
  ECS_LOG("test_event %d %d\n", e.x, y);
}

REQUEST(require = int x)
test_request(MyRequest &r)
{
  r.count += 1;
}
