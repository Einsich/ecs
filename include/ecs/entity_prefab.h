#pragma once
#include <ecs/component_prefab.h>
#include <ecs/chunk_policy.h>

namespace ecs
{
  using prefab_id = uint;
  constexpr prefab_id invalidPrefabId = -1;
  struct EntityPrefab
  {
    ecs::string name;
    ecs::vector<ComponentPrefab> components;
    SizePolicy chunkPower;
    bool requireAwaitCreation = false;
    EntityPrefab() = default;
    EntityPrefab(const char *name, ecs::vector<ComponentPrefab> &&components, SizePolicy chunk_power = SizePolicy::Hundreds);
  };

  prefab_id create_entity_prefab(EntityPrefab &&prefab);
  prefab_id get_prefab_id(const char *name);
  prefab_id get_prefab_id(const ecs::string &name);
  const EntityPrefab &get_prefab(prefab_id id);
}