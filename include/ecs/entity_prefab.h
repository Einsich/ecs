#pragma once
#include <ecs/component_prefab.h>

namespace ecs
{
  struct EntityPrefab
  {
    ecs::string name;
    ecs::vector<ComponentPrefab> components;
    EntityPrefab() = default;
    EntityPrefab(const char *name, ecs::vector<ComponentPrefab> &&components)
        : name(name), components(std::move(components))
    {
      sort_prefabs_by_names(this->components);
    }
  };
}