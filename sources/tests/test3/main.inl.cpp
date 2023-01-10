#include "main.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache stage_render__cache__;

static ecs::QueryCache stage_test1__cache__;

static ecs::QueryCache stage_test2__cache__;

static ecs::QueryCache stage_test3__cache__;

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

static void registration_pull_main()
{
  ecs::register_system(ecs::SystemDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test3/main.inl:19",
  "stage_render",
  &stage_render__cache__,
  {},
  {},
  {},
  "render",
  {},
  {},
  {},
  &stage_render_implementation));

  ecs::register_system(ecs::SystemDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test3/main.inl:25",
  "stage_test1",
  &stage_test1__cache__,
  {},
  {},
  {},
  "main",
  {"stage_test2"},
  {},
  {},
  &stage_test1_implementation));

  ecs::register_system(ecs::SystemDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test3/main.inl:31",
  "stage_test2",
  &stage_test2__cache__,
  {},
  {},
  {},
  "main",
  {},
  {},
  {},
  &stage_test2_implementation));

  ecs::register_system(ecs::SystemDescription(
  "C:/Users/Lord/Documents/ecs/sources/tests/test3/main.inl:37",
  "stage_test3",
  &stage_test3__cache__,
  {},
  {},
  {},
  "main",
  {"stage_test1", "stage_test2"},
  {},
  {},
  &stage_test3_implementation));

}
ECS_FILE_REGISTRATION(&registration_pull_main)
ECS_PULL_DEFINITION(variable_pull_main)
