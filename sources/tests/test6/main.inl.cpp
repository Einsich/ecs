#include "main.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache query__cache__;

static ecs::QueryCache query_with_singleton__cache__;

static ecs::QueryCache ecs_system_iteration__cache__;

static ecs::QueryCache ecs_system_with_singleton_iteration__cache__;

static ecs::QueryCache ecs_query_iteration__cache__;

static ecs::QueryCache ecs_query_with_singleton_iteration__cache__;

template<typename Callable>
static void query(Callable lambda)
{
  ecs::perform_query<vec4&, const vec4&>(query__cache__, lambda);
}

template<typename Callable>
static void query_with_singleton(Callable lambda)
{
  ecs::perform_query<vec4&, const vec4&, const GlobalTime&>(query_with_singleton__cache__, lambda);
}

static void ecs_system_iteration_implementation()
{
  ecs::perform_system(ecs_system_iteration__cache__, ecs_system_iteration);
}

static void ecs_system_with_singleton_iteration_implementation()
{
  ecs::perform_system(ecs_system_with_singleton_iteration__cache__, ecs_system_with_singleton_iteration);
}

static void ecs_query_iteration_implementation()
{
  ecs::perform_system(ecs_query_iteration__cache__, ecs_query_iteration);
}

static void ecs_query_with_singleton_iteration_implementation()
{
  ecs::perform_system(ecs_query_with_singleton_iteration__cache__, ecs_query_with_singleton_iteration);
}

static void registration_pull_main()
{
  ecs::register_query(ecs::QueryDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test6/main.inl:144",
  "query",
  &query__cache__,
  {
    {"pos", ecs::get_type_index<vec4>(), ecs::AccessType::ReadWrite, false, ecs::is_singleton<vec4>()},
    {"vel", ecs::get_type_index<vec4>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<vec4>()}
  },
  {},
  {}
  ));

  ecs::register_query(ecs::QueryDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test6/main.inl:155",
  "query_with_singleton",
  &query_with_singleton__cache__,
  {
    {"pos", ecs::get_type_index<vec4>(), ecs::AccessType::ReadWrite, false, ecs::is_singleton<vec4>()},
    {"vel", ecs::get_type_index<vec4>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<vec4>()},
    {"globalTime", ecs::get_type_index<GlobalTime>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<GlobalTime>()}
  },
  {},
  {}
  ));

  ecs::register_system(ecs::SystemDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test6/main.inl:127",
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
  &ecs_system_iteration_implementation));

  ecs::register_system(ecs::SystemDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test6/main.inl:133",
  "ecs_system_with_singleton_iteration",
  &ecs_system_with_singleton_iteration__cache__,
  {
    {"pos", ecs::get_type_index<vec4>(), ecs::AccessType::ReadWrite, false, ecs::is_singleton<vec4>()},
    {"vel", ecs::get_type_index<vec4>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<vec4>()},
    {"globalTime", ecs::get_type_index<GlobalTime>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<GlobalTime>()}
  },
  {},
  {},
  "",
  {},
  {},
  {},
  &ecs_system_with_singleton_iteration_implementation));

  ecs::register_system(ecs::SystemDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test6/main.inl:141",
  "ecs_query_iteration",
  &ecs_query_iteration__cache__,
  {},
  {},
  {},
  "",
  {},
  {},
  {},
  &ecs_query_iteration_implementation));

  ecs::register_system(ecs::SystemDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test6/main.inl:152",
  "ecs_query_with_singleton_iteration",
  &ecs_query_with_singleton_iteration__cache__,
  {},
  {},
  {},
  "",
  {},
  {},
  {},
  &ecs_query_with_singleton_iteration_implementation));

}
ECS_FILE_REGISTRATION(&registration_pull_main)
ECS_PULL_DEFINITION(variable_pull_main)
