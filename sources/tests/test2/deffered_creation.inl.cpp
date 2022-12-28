#include "deffered_creation.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache get_next_node__cache__;

static ecs::QueryCache no_arguments__cache__;

static ecs::QueryCache test_deffered_creation__cache__;

template<typename Callable>
static void get_next_node(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<ecs::EntityId, float>(get_next_node__cache__, eid, lambda);
}

static void no_arguments_implementation()
{
  ecs::perform_system(no_arguments__cache__, no_arguments);
}

static void test_deffered_creation_implementation()
{
  ecs::perform_system(test_deffered_creation__cache__, test_deffered_creation);
}

static void registration_pull_deffered_creation()
{
  ecs::register_query(ecs::QueryDescription(
  "",
  "get_next_node",
  &get_next_node__cache__,
  {
    {"nextEid", ecs::get_type_index<ecs::EntityId>(), ecs::AccessType::Copy, false, ecs::is_singleton<ecs::EntityId>()},
    {"value", ecs::get_type_index<float>(), ecs::AccessType::Copy, false, ecs::is_singleton<float>()}
  },
  {},
  {}
  ));

  ecs::register_system(ecs::SystemDescription(
  "",
  "no_arguments",
  &no_arguments__cache__,
  {},
  {},
  {},
  "",
  {},
  {},
  {},
  &no_arguments_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "test_deffered_creation",
  &test_deffered_creation__cache__,
  {
    {"value", ecs::get_type_index<float>(), ecs::AccessType::Copy, false, ecs::is_singleton<float>()},
    {"eid", ecs::get_type_index<ecs::EntityId>(), ecs::AccessType::Copy, false, ecs::is_singleton<ecs::EntityId>()},
    {"nextEid", ecs::get_type_index<ecs::EntityId>(), ecs::AccessType::Copy, false, ecs::is_singleton<ecs::EntityId>()}
  },
  {},
  {},
  "",
  {},
  {},
  {},
  &test_deffered_creation_implementation));

}
ECS_FILE_REGISTRATION(&registration_pull_deffered_creation)
ECS_PULL_DEFINITION(variable_pull_deffered_creation)
