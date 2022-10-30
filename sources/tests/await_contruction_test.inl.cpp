#include "await_contruction_test.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache get_next_node__cache__;

static ecs::QueryCache test_awaited_creation__cache__;

static ecs::QueryCache test_event__cache__;

template<typename Callable>
static void get_next_node(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<ecs::EntityId, int>(get_next_node__cache__, eid, lambda);
}

static void test_awaited_creation_implementation()
{
  ecs::perform_system(test_awaited_creation__cache__, test_awaited_creation);
}

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
  ecs::register_query(ecs::QueryDescription(
  "",
  "get_next_node",
  &get_next_node__cache__,
  {
    {"nextEid", ecs::TypeIndex<ecs::EntityId>::value, ecs::AccessType::Copy, false},
    {"value", ecs::TypeIndex<int>::value, ecs::AccessType::Copy, false}
  },
  {},
  {}
  ));

  ecs::register_system(ecs::SystemDescription(
  "",
  "test_awaited_creation",
  &test_awaited_creation__cache__,
  {
    {"value", ecs::TypeIndex<int>::value, ecs::AccessType::Copy, false},
    {"eid", ecs::TypeIndex<ecs::EntityId>::value, ecs::AccessType::Copy, false}
  },
  {
    {"nextEid", ecs::TypeIndex<ecs::EntityId>::value},
    {"value", ecs::TypeIndex<int>::value}
},
  {},
  {},
  {},
  {},
  &test_awaited_creation_implementation));

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
