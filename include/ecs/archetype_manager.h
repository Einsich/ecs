#pragma once
#include <ecs/archetype.h>
#include <ecs/entity_id_pool.h>
#include <ecs/entity_prefab.h>

namespace ecs
{
  using archetype_id = uint;
  constexpr archetype_id invalidArchetypeId = -1;
  struct ArchetypeManager
  {
    ecs::vector<Archetype> archetypes;
    ecs::vector<archetype_id> prefabArchetypeCache; // cache for every prefab_id
    EntityPool entityPool;
    struct DefferedEntityCreation
    {
      ecs::vector<ComponentPrefab> overrides_list;
      prefab_id prefab;
      EntityDescription *entity;
    };
    ecs::queue<DefferedEntityCreation> defferedEntityCreation;

    void clear_prefab_cache()
    {
      prefabArchetypeCache.clear();
    }

    archetype_id get_archetype_for_prefab(prefab_id id)
    {
      if (id < prefabArchetypeCache.size() && prefabArchetypeCache[id] != invalidArchetypeId)
        return prefabArchetypeCache[id];
      else
        return invalidArchetypeId;
    }
    void add_prefab_archetype_cache(prefab_id id, archetype_id archetype_id)
    {
      if (id < prefabArchetypeCache.size())
      {
        ECS_ASSERT(prefabArchetypeCache[id] == invalidArchetypeId);
        prefabArchetypeCache[id] = archetype_id;
      }
      else
      {
        prefabArchetypeCache.resize(id + 1, invalidArchetypeId);
        prefabArchetypeCache.back() = id;
      }
    }
  };

  inline ArchetypeManager &get_archetype_manager()
  {
    static ArchetypeManager manager;
    return manager;
  }
}