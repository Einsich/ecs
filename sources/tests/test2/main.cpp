#include <ecs/ecs.h>
#include "../tests.h"

int main()
{
  ecs::init();
  ecs::pull_registered_files();

  ecs::perform_stage("");
  ecs::update_archetype_manager();
  ecs::perform_stage("");

  ecs::destroy_all_entities();
  std::fflush(stdout);
  return 0;
}
