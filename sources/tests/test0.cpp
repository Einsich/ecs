#include <ecs/a.h>
#include <ecs/ecs.h>
#include "test0.h"

ECS_EVENT_REGISTRATION(MyEvent, "MyEvent", true)
ECS_REQUEST_REGISTRATION(MyRequest, "MyRequest")

int main()
{
  ecs::init();

  extern void registration_pull_test0_es();
  registration_pull_test0_es();

  ecs::EntityPrefab p("lol", {{"x", 1.f}, {"y", 2.f}, {"z", 3}});

  ecs::create_entity_immediate(p);
  ecs::create_entity_immediate(p, {{"x", 2.f}, {"y", -2.f}});
  ecs::create_entity_immediate(p, {{"x", 3.f}, {"z", -2.f}});
  ecs::create_entity_immediate(p, {{"x", 4.f}, {"x", -4.f}});

  ecs::update_query_manager();
  ecs::perform_systems();

  void events_testing();
  events_testing();
  void requests_testing();
  requests_testing();
  f();
  return 0;
}

struct A
{
};

struct B
{
  char b;
};
struct C
{
  int c;
};

static_assert(ecs::is_zero_sizeof<A> == true);
static_assert(ecs::is_zero_sizeof<B> == false);
static_assert(ecs::is_zero_sizeof<C> == false);

void events_testing()
{

  ecs::update_query_manager(); // after register_event
  MyEvent m;
  m.x = 69;
  ecs::send_event_immediate(m);
  m.x = 96;
  ecs::send_event(m);
  ecs::update_query_manager();
}

void requests_testing()
{

  ecs::update_query_manager(); // after register_request
  MyRequest r;
  ecs::send_request(r);
  printf("request count %d\n", r.count);
}