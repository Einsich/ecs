#include "main.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache query__cache__;

static ecs::QueryCache array_iteration__cache__;

static ecs::QueryCache array_ptr_iteration__cache__;

static ecs::QueryCache array_i_ptr_iteration__cache__;

static ecs::QueryCache ecs_system_iteration__cache__;

static ecs::QueryCache ecs_query_iteration__cache__;

template<typename Callable>
static void query(Callable lambda)
{
  ecs::perform_query<vec4&, const vec4&>(query__cache__, lambda);
}

static void array_iteration_implementation()
{
  ecs::perform_system(array_iteration__cache__, array_iteration);
}

static void array_ptr_iteration_implementation()
{
  ecs::perform_system(array_ptr_iteration__cache__, array_ptr_iteration);
}

static void array_i_ptr_iteration_implementation()
{
  ecs::perform_system(array_i_ptr_iteration__cache__, array_i_ptr_iteration);
}

static void ecs_system_iteration_implementation()
{
  ecs::perform_system(ecs_system_iteration__cache__, ecs_system_iteration);
}

static void ecs_query_iteration_implementation()
{
  ecs::perform_system(ecs_query_iteration__cache__, ecs_query_iteration);
}

static void registration_pull_main()
{
  ecs::register_query(ecs::QueryDescription(
  "",
  "query",
  &query__cache__,
  {
    {"pos", ecs::TypeIndex<vec4>::value, ecs::AccessType::ReadWrite, false},
    {"vel", ecs::TypeIndex<vec4>::value, ecs::AccessType::ReadOnly, false}
  },
  {},
  {}
  ));

  ecs::register_system(ecs::SystemDescription(
  "",
  "array_iteration",
  &array_iteration__cache__,
  {},
  {},
  {},
  {},
  {},
  {},
  &array_iteration_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "array_ptr_iteration",
  &array_ptr_iteration__cache__,
  {},
  {},
  {},
  {},
  {},
  {},
  &array_ptr_iteration_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "array_i_ptr_iteration",
  &array_i_ptr_iteration__cache__,
  {},
  {},
  {},
  {},
  {},
  {},
  &array_i_ptr_iteration_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "ecs_system_iteration",
  &ecs_system_iteration__cache__,
  {
    {"pos", ecs::TypeIndex<vec4>::value, ecs::AccessType::ReadWrite, false},
    {"vel", ecs::TypeIndex<vec4>::value, ecs::AccessType::ReadOnly, false}
  },
  {},
  {},
  {},
  {},
  {},
  &ecs_system_iteration_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "ecs_query_iteration",
  &ecs_query_iteration__cache__,
  {},
  {},
  {},
  {},
  {},
  {},
  &ecs_query_iteration_implementation));

}
ECS_FILE_REGISTRATION(&registration_pull_main)
