#include <ecs/type_registration.h>
#include <ecs/entity_id.h>

namespace ecs
{
  void init(UserFunctions entity_id_functions)
  {
    type_registration<ecs::EntityId, true, true, true, true, false>("eid", entity_id_functions);
  }
}