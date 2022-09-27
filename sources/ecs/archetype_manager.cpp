#include <ecs/archetype_manager.h>

namespace ecs
{

  static uint add_archetype_impl(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power)
  {
    uint result = 0;
    for (const auto &archetype : get_archetype_manager().archetypes)
    {
      result++;
      if (archetype.components.size() != descriptions.size())
        continue;
      for (uint i = 0, n = descriptions.size(); i < n; i++)
        if (!archetype.components[i].description.fastCompare(descriptions[i]))
          continue;
      if (archetype.chunkPower != (uint)chunk_power)
      {
        ECS_ERROR("missmatch in chunk sizes");
      }
      return result - 1;
    }
    get_archetype_manager().archetypes.emplace_back(std::move(descriptions), chunk_power);
    return result;
  }

  uint add_archetype(const ecs::vector<ComponentPrefab> &prefabs, SizePolicy chunk_power)
  {
    ecs::vector<ComponentDescription> descriptions(prefabs.size());
    for (uint i = 0, n = prefabs.size(); i < n; i++)
      descriptions[i] = prefabs[i];
    return add_archetype_impl(std::move(descriptions), chunk_power);
  }
  uint add_archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power)
  {
    return add_archetype_impl(std::move(descriptions), chunk_power);
  }
  void create_entity_immediate(const EntityPrefab &prefabs_list, EntityPrefab &&overrides_list, SizePolicy chunk_power)
  {
    uint archetype = add_archetype(prefabs_list.components, chunk_power);
    //need to validate components with async creation here.
    //also added async creation
    get_archetype_manager().archetypes[archetype].add_entity(prefabs_list, std::move(overrides_list));
  }
}