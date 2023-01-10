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
  ecs::perform_query<int&, int&>(test_single_query__cache__, eid, lambda);
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
  "C:/Users/Lord/Documents/ecs/sources/tests/test0/main.inl:55",
  "test_single_query",
  &test_single_query__cache__,
  {
    {"z", ecs::get_type_index<int>(), ecs::AccessType::ReadWrite, false, ecs::is_singleton<int>()},
    {"y", ecs::get_type_index<int>(), ecs::AccessType::ReadWrite, false, ecs::is_singleton<int>()}
  },
  {},
  {}
  ));

  ecs::register_system(ecs::SystemDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test0/main.inl:49",
  "test",
  &test__cache__,
  {
    {"x", ecs::get_type_index<int>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<int>()},
    {"y", ecs::get_type_index<int>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<int>()},
    {"z", ecs::get_type_index<int>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<int>()},
    {"eid", ecs::get_type_index<ecs::EntityId>(), ecs::AccessType::Copy, false, ecs::is_singleton<ecs::EntityId>()}
  },
  {},
  {},
  "",
  {},
  {},
  {},
  &test_implementation));

  ecs::register_event(ecs::EventDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test0/main.inl:62",
  "test_event",
  &test_event__cache__,
  {
    {"y", ecs::get_type_index<int>(), ecs::AccessType::Copy, false, ecs::is_singleton<int>()}
  },
  {},
  {},
  {},
  {},
  {},
  &test_event_handler, &test_event_single_handler),
  ecs::EventIndex<MyEvent>::value);

  ecs::register_request(ecs::RequestDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test0/main.inl:68",
  "test_request",
  &test_request__cache__,
  {},
  {
    {"x", ecs::TypeIndex<int>::value}
  },
  {},
  {},
  {},
  {},
  &test_request_handler, &test_request_single_handler),
  ecs::RequestIndex<MyRequest>::value);

}
ECS_FILE_REGISTRATION(&registration_pull_main)
ECS_PULL_DEFINITION(variable_pull_main)
