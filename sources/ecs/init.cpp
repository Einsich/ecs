#include <ecs/type_registration.h>
#include <ecs/entity_id.h>
#include <ecs/base_events.h>
#include <ecs/query_manager.h>

namespace ecs
{
  void init(UserFunctions entity_id_functions)
  {
    type_registration<ecs::EntityId, true, true, true>("EntityId", entity_id_functions, {});
    register_event<ecs::OnEntityCreated>("OnEntityCreated", true);
    register_event<ecs::OnEntityDestoyed>("OnEntityDestoyed", true);
    register_event<ecs::OnEntityTerminated>("OnEntityTerminated", true);
    register_event<ecs::OnSceneCreated>("OnSceneCreated", true);
    register_event<ecs::OnSceneTerminated>("OnSceneTerminated", true);
  }

  void pull_registered_files()
  {
    extern void register_all_pulled_files();
    register_all_pulled_files();

    extern void update_query_manager();
    update_query_manager();
  }
}