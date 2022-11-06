#include "test0_es.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache test_single_query__cache__;

static ecs::QueryCache get_next_node__cache__;

static ecs::QueryCache test__cache__;

static ecs::QueryCache no_arguments__cache__;

static ecs::QueryCache test_deffered_creation__cache__;

static ecs::QueryCache stage_render__cache__;

static ecs::QueryCache stage_test1__cache__;

static ecs::QueryCache stage_test2__cache__;

static ecs::QueryCache stage_test3__cache__;

static ecs::QueryCache test_event__cache__;

static ecs::QueryCache test_request__cache__;

template<typename Callable>
static void test_single_query(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<int&, float&>(test_single_query__cache__, eid, lambda);
}

template<typename Callable>
static void get_next_node(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<ecs::EntityId, float>(get_next_node__cache__, eid, lambda);
}

static void test_implementation()
{
  ecs::perform_system(test__cache__, test);
}

static void no_arguments_implementation()
{
  ecs::perform_system(no_arguments__cache__, no_arguments);
}

static void test_deffered_creation_implementation()
{
  ecs::perform_system(test_deffered_creation__cache__, test_deffered_creation);
}

static void stage_render_implementation()
{
  ecs::perform_system(stage_render__cache__, stage_render);
}

static void stage_test1_implementation()
{
  ecs::perform_system(stage_test1__cache__, stage_test1);
}

static void stage_test2_implementation()
{
  ecs::perform_system(stage_test2__cache__, stage_test2);
}

static void stage_test3_implementation()
{
  ecs::perform_system(stage_test3__cache__, stage_test3);
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

void registration_pull_test0_es()
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

  ecs::register_query(ecs::QueryDescription(
  "",
  "get_next_node",
  &get_next_node__cache__,
  {
    {"nextEid", ecs::TypeIndex<ecs::EntityId>::value, ecs::AccessType::Copy, false},
    {"value", ecs::TypeIndex<float>::value, ecs::AccessType::Copy, false}
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

  ecs::register_system(ecs::SystemDescription(
  "",
  "no_arguments",
  &no_arguments__cache__,
  {},
  {},
  {},
  {},
  {},
  {},
  &no_arguments_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "test_deffered_creation",
  &test_deffered_creation__cache__,
  {
    {"value", ecs::TypeIndex<float>::value, ecs::AccessType::Copy, false},
    {"eid", ecs::TypeIndex<ecs::EntityId>::value, ecs::AccessType::Copy, false},
    {"nextEid", ecs::TypeIndex<ecs::EntityId>::value, ecs::AccessType::Copy, false}
  },
  {},
  {},
  {},
  {},
  {},
  &test_deffered_creation_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "stage_render",
  &stage_render__cache__,
  {},
  {},
  {},
  {"render_before_sync_point", "stage_test3"},
  {"render_after_sync_point"},
  {},
  &stage_render_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "stage_test1",
  &stage_test1__cache__,
  {},
  {},
  {},
  {"main_before_sync_point", "stage_test2"},
  {"main_after_sync_point"},
  {},
  &stage_test1_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "stage_test2",
  &stage_test2__cache__,
  {},
  {},
  {},
  {"main_before_sync_point"},
  {"main_after_sync_point"},
  {},
  &stage_test2_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "stage_test3",
  &stage_test3__cache__,
  {},
  {},
  {},
  {"main_before_sync_point", "stage_test1", "stage_test2"},
  {"main_after_sync_point"},
  {},
  &stage_test3_implementation));

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
  {},
  {},
  {},
  {},
  {},
  &test_request_handler, &test_request_single_handler),
  ecs::RequestIndex<MyRequest>::value);

}
