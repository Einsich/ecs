#include <ecs/type_annotation.h>
#include <ecs/entity_prefab.h>
#include <ecs/entity_id.h>

namespace ecs
{
  EntityPrefab::EntityPrefab(const char *name, ecs::vector<ComponentPrefab> &&components_)
      : name(name), components(std::move(components_))
  {
    static ComponentPrefab entityIdPrefab = ComponentPrefab("eid", EntityId());
    components.push_back(entityIdPrefab);
    sort_prefabs_by_names(components);

    bool have_collisions = false;
    for (uint i = 1, n = components.size(); i < n; i++)
    {
      if (components[i].nameHash == components[i - 1].nameHash)
      {
        have_collisions = true;
        ECS_ERROR("components with same name \"%s\" in \"%s\" prefab, with types <%s> <%s>",
                  name,
                  components[i].name.c_str(),
                  type_name(components[i].typeIndex),
                  type_name(components[i - 1].typeIndex));
      }
    }
    if (have_collisions)
      components.clear();
  }
}