#include "main.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache query__cache__;

static ecs::QueryCache ecs_query_iteration__cache__;

template<typename Callable>
static void query(Callable lambda)
{
  ecs::perform_query<>(query__cache__, lambda);
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
  {},
  {},
  {}
  ));

  ecs::register_system(ecs::SystemDescription(
  "",
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

}
ECS_FILE_REGISTRATION(&registration_pull_main)
ECS_PULL_DEFINITION(variable_pull_main)
