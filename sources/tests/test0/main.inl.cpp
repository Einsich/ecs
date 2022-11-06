#include "main.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache test_single_query__cache__;

static ecs::QueryCache test__cache__;

static ecs::QueryCache test_event__cache__;

static ecs::QueryCache test_request__cache__;

template<typename Callable>
static void test_single_query(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<int&, float&>(test_single_query__cache__, eid, lambda);
}

static void test_implementation()
{
  ecs::perform_system(test__cache__, test);
}

static void test_event_handler(const ecs::Event &event)
{
  ecs::perform_event(reinterpret_cast<const MyEvent &>(event), test_event__cache__, test_event);
}

static void test_event_single_handler(ecs::EntityId eid, const ecs::Event &event)
{
  ecs::perform_event(eid, reinterpret_cast<const MyEvent &>(event), test_event__cache__, test_event);
}

static void test_request_handler(ecs::Request &request)
{
  ecs::perform_request(reinterpret_cast<MyRequest &>(request), test_request__cache__, test_request);
}

static void test_request_single_handler(ecs::EntityId eid, ecs::Request &request)
{
  ecs::perform_request(eid, reinterpret_cast<MyRequest &>(request), test_request__cache__, test_request);
}

static void registration_pull_main()
{
  ecs::register_query(ecs::QueryDescription(
  "",
  "test_single_query",
  &test_single_query__cache__,
  {
    {"z", ecs::TypeIndex<int>::value, ecs::AccessType::ReadWrite, false},
    {"y", ecs::TypeIndex<float>::value, ecs::AccessType::ReadWrite, false}
  },
  {},
  {}
  ));

  ecs::register_system(ecs::SystemDescription(
  "",
  "test",
  &test__cache__,
  {
    {"x", ecs::TypeIndex<float>::value, ecs::AccessType::ReadOnly, false},
    {"y", ecs::TypeIndex<float>::value, ecs::AccessType::ReadOnly, false},
    {"z", ecs::TypeIndex<int>::value, ecs::AccessType::ReadOnly, false},
    {"eid", ecs::TypeIndex<ecs::EntityId>::value, ecs::AccessType::Copy, false}
  },
  {},
  {},
  {},
  {},
  {},
  &test_implementation));

  ecs::register_event(ecs::EventDescription(
  "",
  "test_event",
  &test_event__cache__,
  {
    {"y", ecs::TypeIndex<float>::value, ecs::AccessType::Copy, false}
  },
  {},
  {},
  {},
  {},
  {},
  &test_event_handler, &test_event_single_handler),
  ecs::EventIndex<MyEvent>::value);

  ecs::register_request(ecs::RequestDescription(
  "",
  "test_request",
  &test_request__cache__,
  {},
  {
    {"x", ecs::TypeIndex<float>::value}
  },
  {},
  {},
  {},
  {},
  &test_request_handler, &test_request_single_handler),
  ecs::RequestIndex<MyRequest>::value);

}
ECS_FILE_REGISTRATION(&registration_pull_main)
