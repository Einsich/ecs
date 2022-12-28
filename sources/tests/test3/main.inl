#include <ecs/ecs.h>
#include "../tests.h"

static void pre_main()
{
  ECS_LOG("pre_main");
}

static void post_main()
{
  ECS_LOG("post_main");
}

int main()
{
  ecs::init();
  ecs::init_stages({{"main", &pre_main, &post_main}, {"render", nullptr, nullptr}});
  ecs::pull_registered_files();

  ecs::perform_systems();

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