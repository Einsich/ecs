#include "await_contruction_test.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache get_next_node__cache__;

static ecs::QueryCache test_event__cache__;

static ecs::QueryCache test_awaited_creation__cache__;

static ecs::QueryCache test_awaited_delete__cache__;

static ecs::QueryCache test_awaited_terminate__cache__;

template<typename Callable>
static void get_next_node(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<ecs::EntityId, int>(get_next_node__cache__, eid, lambda);
}

static void test_event_handler(const ecs::Event &event)
{
  ecs::perform_event(reinterpret_cast<const PrepareTest &>(event), test_event__cache__, test_event);
}

static void test_event_single_handler(ecs::EntityId eid, const ecs::Event &event)
{
  ecs::perform_event(eid, reinterpret_cast<const PrepareTest &>(event), test_event__cache__, test_event);
}

static void test_awaited_creation_handler(const ecs::Event &event)
{
  ecs::perform_event(reinterpret_cast<const ecs::OnEntityCreated &>(event), test_awaited_creation__cache__, test_awaited_creation);
}

static void test_awaited_creation_single_handler(ecs::EntityId eid, const ecs::Event &event)
{
  ecs::perform_event(eid, reinterpret_cast<const ecs::OnEntityCreated &>(event), test_awaited_creation__cache__, test_awaited_creation);
}

static void test_awaited_delete_handler(const ecs::Event &event)
{
  ecs::perform_event(reinterpret_cast<const ecs::OnEntityDestoyed &>(event), test_awaited_delete__cache__, test_awaited_delete);
}

static void test_awaited_delete_single_handler(ecs::EntityId eid, const ecs::Event &event)
{
  ecs::perform_event(eid, reinterpret_cast<const ecs::OnEntityDestoyed &>(event), test_awaited_delete__cache__, test_awaited_delete);
}

static void test_awaited_terminate_handler(const ecs::Event &event)
{
  ecs::perform_event(reinterpret_cast<const ecs::OnEntityTerminated &>(event), test_awaited_terminate__cache__, test_awaited_terminate);
}

static void test_awaited_terminate_single_handler(ecs::EntityId eid, const ecs::Event &event)
{
  ecs::perform_event(eid, reinterpret_cast<const ecs::OnEntityTerminated &>(event), test_awaited_terminate__cache__, test_awaited_terminate);
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

  ecs::register_event(ecs::EventDescription(
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
  &test_awaited_creation_handler, &test_awaited_creation_single_handler),
  ecs::EventIndex<ecs::OnEntityCreated>::value);

  ecs::register_event(ecs::EventDescription(
  "",
  "test_awaited_delete",
  &test_awaited_delete__cache__,
  {
    {"value", ecs::TypeIndex<int>::value, ecs::AccessType::Copy, false}
  },
  {
    {"nextEid", ecs::TypeIndex<ecs::EntityId>::value}
},
  {},
  {},
  {},
  {},
  &test_awaited_delete_handler, &test_awaited_delete_single_handler),
  ecs::EventIndex<ecs::OnEntityDestoyed>::value);

  ecs::register_event(ecs::EventDescription(
  "",
  "test_awaited_terminate",
  &test_awaited_terminate__cache__,
  {
    {"value", ecs::TypeIndex<int>::value, ecs::AccessType::Copy, false}
  },
  {
    {"nextEid", ecs::TypeIndex<ecs::EntityId>::value}
},
  {},
  {},
  {},
  {},
  &test_awaited_terminate_handler, &test_awaited_terminate_single_handler),
  ecs::EventIndex<ecs::OnEntityTerminated>::value);

}