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
  "",
  "stage_render",
  &stage_render__cache__,
  {},
  {},
  {},
  {"render_end_sync_point"},
  {"render_begin_sync_point"},
  {},
  &stage_render_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "stage_test1",
  &stage_test1__cache__,
  {},
  {},
  {},
  {"main_end_sync_point", "stage_test2"},
  {"main_begin_sync_point"},
  {},
  &stage_test1_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "stage_test2",
  &stage_test2__cache__,
  {},
  {},
  {},
  {"main_end_sync_point"},
  {"main_begin_sync_point"},
  {},
  &stage_test2_implementation));

  ecs::register_system(ecs::SystemDescription(
  "",
  "stage_test3",
  &stage_test3__cache__,
  {},
  {},
  {},
  {"main_end_sync_point", "stage_test1", "stage_test2"},
  {"main_begin_sync_point"},
  {},
  &stage_test3_implementation));

}
ECS_FILE_REGISTRATION(&registration_pull_main)
