#include <ecs/type_annotation.h>
#include <ecs/entity_prefab.h>
#include <ecs/entity_id.h>

namespace ecs
{
  EntityPrefab::EntityPrefab(const char *name, ecs::vector<ComponentPrefab> &&components)
      : name(name), components(std::move(components))
  {
    static ComponentPrefab entityIdPrefab = ComponentPrefab("eid", EntityId());
    this->components.push_back(entityIdPrefab);
    sort_prefabs_by_names(this->components);
  }
}