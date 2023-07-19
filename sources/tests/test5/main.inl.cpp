#include "main.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache query__cache__;

static ecs::QueryCache array_iteration__cache__;

static ecs::QueryCache array_ptr_iteration__cache__;

static ecs::QueryCache ecs_system_iteration__cache__;

static ecs::QueryCache ecs_query_iteration__cache__;

static ecs::QueryCache soa_iteration__cache__;

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

static void ecs_system_iteration_implementation()
{
  ecs::perform_system(ecs_system_iteration__cache__, ecs_system_iteration);
}

static void ecs_query_iteration_implementation()
{
  ecs::perform_system(ecs_query_iteration__cache__, ecs_query_iteration);
}

static void soa_iteration_implementation()
{
  ecs::perform_system(soa_iteration__cache__, soa_iteration);
}

static void registration_pull_main()
{
  ecs::register_query(
  "sources/tests/test5/main.inl:223",
  "query",
  &query__cache__,
  {
    {"pos", ecs::get_type_index<vec4>(), ecs::AccessType::ReadWrite, false, ecs::is_singleton<vec4>()},
    {"vel", ecs::get_type_index<vec4>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<vec4>()}
  },
  {},
  {}
  );

  ecs::register_system(
  "sources/tests/test5/main.inl:193",
  "array_iteration",
  &array_iteration__cache__,
  {},
  {},
  {},
  "",
  {},
  {},
  {},
  &array_iteration_implementation);

  ecs::register_system(
  "sources/tests/test5/main.inl:202",
  "array_ptr_iteration",
  &array_ptr_iteration__cache__,
  {},
  {},
  {},
  "",
  {},
  {},
  {},
  &array_ptr_iteration_implementation);

  ecs::register_system(
  "sources/tests/test5/main.inl:211",
  "ecs_system_iteration",
  &ecs_system_iteration__cache__,
  {
    {"pos", ecs::get_type_index<vec4>(), ecs::AccessType::ReadWrite, false, ecs::is_singleton<vec4>()},
    {"vel", ecs::get_type_index<vec4>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<vec4>()}
  },
  {},
  {},
  "",
  {},
  {},
  {},
  &ecs_system_iteration_implementation);

  ecs::register_system(
  "sources/tests/test5/main.inl:220",
  "ecs_query_iteration",
  &ecs_query_iteration__cache__,
  {},
  {},
  {},
  "",
  {},
  {},
  {},
  &ecs_query_iteration_implementation);

  ecs::register_system(
  "sources/tests/test5/main.inl:228",
  "soa_iteration",
  &soa_iteration__cache__,
  {},
  {},
  {},
  "",
  {},
  {},
  {},
  &soa_iteration_implementation);

}
ECS_FILE_REGISTRATION(&registration_pull_main)
ECS_PULL_DEFINITION(variable_pull_main)
