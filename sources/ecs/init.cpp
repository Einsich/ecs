#include <ecs/type_registration.h>
#include <ecs/entity_id.h>
#include <ecs/base_events.h>

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
}