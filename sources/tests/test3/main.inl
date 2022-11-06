#include <ecs/ecs.h>
#include "../tests.h"

static void pre_main()
{
  printf("pre_main\n");
}

static void post_main()
{
  printf("post_main\n");
}

int main()
{
  ecs::init();
  ecs::init_stages({{"main", &pre_main, &post_main}, {"render", nullptr, nullptr}});
  ecs::pull_registered_files();

  ecs::perform_systems();

  ecs::destroy_all_entities();
  return 0;
}

SYSTEM(stage = render)
stage_render()
{
  printf("stage_render\n");
}

SYSTEM(stage = main; before = stage_test2)
stage_test1()
{
  printf("stage_test1\n");
}

SYSTEM(stage = main)
stage_test2()
{
  printf("stage_test2\n");
}

SYSTEM(stage = main; before = stage_test1, stage_test2)
stage_test3()
{
  printf("stage_test3\n");
}