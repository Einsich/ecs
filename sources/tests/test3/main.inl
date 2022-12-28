#include <ecs/ecs.h>
#include "../tests.h"

int main()
{
  ecs::init();
  ecs::pull_registered_files();

  ECS_LOG("pre_main");
  ecs::perform_stage("main");
  ECS_LOG("post_main");
  ecs::perform_stage("render");

  ecs::destroy_all_entities();
  std::fflush(stdout);
  return 0;
}

SYSTEM(stage = render)
stage_render()
{
  ECS_LOG("stage_render");
}

SYSTEM(stage = main; before = stage_test2)
stage_test1()
{
  ECS_LOG("stage_test1");
}

SYSTEM(stage = main)
stage_test2()
{
  ECS_LOG("stage_test2");
}

SYSTEM(stage = main; before = stage_test1, stage_test2)
stage_test3()
{
  ECS_LOG("stage_test3");
}