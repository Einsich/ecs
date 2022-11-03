#include <ecs/a.h>
#include <ecs/ecs.h>
#include "test0.h"

ECS_EVENT_REGISTRATION(PrepareTest, "PrepareTest", true)
ECS_EVENT_REGISTRATION(MyEvent, "MyEvent", true)
ECS_REQUEST_REGISTRATION(MyRequest, "MyRequest")

int main()
{
  ecs::init();

  extern void registration_pull_test0_es();
  registration_pull_test0_es();

  extern void registration_pull_await_contruction_test();
  registration_pull_await_contruction_test();

  ecs::prefab_id p = ecs::create_entity_prefab({"lol", {{"x", 1.f}, {"y", 2.f}, {"z", 3}}});

  auto eid1 = ecs::create_entity_immediate(p);
  ecs::create_entity_immediate(p, {{"x", 3.f}, {"z", -2.f}});

  ecs::update_query_manager(); // after register_event, register_request, etc
  ecs::send_event_immediate(PrepareTest());
  ecs::perform_systems();

  ecs::destroy_entity(eid1);
  ecs::destroy_entity(eid1);
  ecs::update_archetype_manager();
  for (int i = 0; i < 3; i++)
  {
    ecs::update_archetype_manager();
  }
  ecs::perform_systems();

  // void events_requests_testing();
  // events_requests_testing();
  f();
  ecs::destroy_all_entities();
  return 0;
}

void events_requests_testing()
{
  MyEvent m;
  m.x = 69;
  ecs::send_event_immediate(m);
  m.x = 96;
  ecs::send_event(m);
  ecs::update_query_manager();

  MyRequest r;
  ecs::send_request(r);
  printf("request count %d\n", r.count);
}