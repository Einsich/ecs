#include <ecs/ecs.h>
#include <ecs/event_registration.h>
#include "../tests.h"

ECS_EVENT_REGISTRATION(PrepareTest)

int main()
{
  ecs::init();
  ecs::pull_registered_files();

  ecs::send_event_immediate(PrepareTest());
  ecs::perform_systems();

  ecs::update_archetype_manager();
  for (int i = 0; i < 3; i++)
  {
    ecs::update_archetype_manager();
  }
  ecs::perform_systems();

  ecs::destroy_all_entities();
  std::fflush(stdout);
  return 0;
}
