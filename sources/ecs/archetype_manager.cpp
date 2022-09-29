#include <ecs/archetype_manager.h>
#include <ecs/query_manager.h>

namespace ecs
{
  template <typename Description>
  static int find_archetype(const ecs::vector<Description> &descriptions)
  {
    int result = 0;
    for (const auto &archetype : get_archetype_manager().archetypes)
    {
      result++;
      if (archetype.components.size() != descriptions.size())
        continue;
      for (uint i = 0, n = descriptions.size(); i < n; i++)
        if (!archetype.components[i].description.fastCompare(descriptions[i]))
          continue;
      return result - 1;
    }
    return -1;
  }

  uint add_archetype(const ecs::vector<ComponentPrefab> &prefabs, SizePolicy chunk_power)
  {
    int idx = find_archetype(prefabs);
    if (idx < 0)
    {
      ecs::vector<ComponentDescription> descriptions(prefabs.size());
      for (uint i = 0, n = prefabs.size(); i < n; i++)
        descriptions[i] = prefabs[i];
      idx = get_archetype_manager().archetypes.size();
      get_archetype_manager().archetypes.emplace_back(std::move(descriptions), chunk_power);
      get_query_manager().addArchetypeToCache(idx);
    }
    else
    {
      if (get_archetype_manager().archetypes[idx].chunkPower != (uint)chunk_power)
      {
        ECS_ERROR("missmatch in chunk sizes");
      }
    }
    return idx;
  }
  uint add_archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power)
  {
    int idx = find_archetype(descriptions);
    if (idx < 0)
    {
      idx = get_archetype_manager().archetypes.size();
      get_archetype_manager().archetypes.emplace_back(std::move(descriptions), chunk_power);
      get_query_manager().addArchetypeToCache(idx);
    }
    else
    {
      if (get_archetype_manager().archetypes[idx].chunkPower != (uint)chunk_power)
      {
        ECS_ERROR("missmatch in chunk sizes");
      }
    }
    return idx;
  }
  void create_entity_immediate(const EntityPrefab &prefabs_list, EntityPrefab &&overrides_list, SizePolicy chunk_power)
  {
    uint archetype = add_archetype(prefabs_list.components, chunk_power);
    // need to validate components with async creation here.
    // also added async creation
    get_archetype_manager().archetypes[archetype].add_entity(prefabs_list, std::move(overrides_list));
  }
}