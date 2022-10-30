#include "await_contruction_test.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache test_event__cache__;

static void test_event_handler(const ecs::Event &event)
{
  ecs::perform_event(reinterpret_cast<const PrepareTest &>(event), test_event__cache__, test_event);
}

static void test_event_single_handler(ecs::EntityId eid, const ecs::Event &event)
{
  ecs::perform_event(eid, reinterpret_cast<const PrepareTest &>(event), test_event__cache__, test_event);
}

void registration_pull_await_contruction_test()
{
  ecs::register_event(ecs::EventDescription(
  "",
  "test_event",
  &test_event__cache__,
  {},
  {},
  {},
  {},
  {},
  {},
  &test_event_handler, &test_event_single_handler),
  ecs::EventIndex<PrepareTest>::value);

}
