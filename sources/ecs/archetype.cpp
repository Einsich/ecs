#include <ecs/archetype.h>
#include <ecs/type_annotation.h>

namespace ecs
{
  Archetype::Archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power)
      : chunkPower((uint)chunk_power), chunkMask((1u << (uint)chunk_power) - 1u), chunkSize(1u << (uint)chunk_power)
  {
    components.resize(descriptions.size());
    sort_descriptions_by_names(descriptions);
    int i = 0;
    for (ComponentContainer &container : components)
    {
      container.description = std::move(descriptions[i]);
      i++;
    }
  }

  int Archetype::findComponentIndex(const ComponentDescription &descr) const
  {
    if (components.empty())
      return -1;
    uint l = 0, r = components.size();

    while (r - l > 1)
    { // divide and conquer, find half that contains answer
      uint m = (l + r) / 2u;
      if (component_comparator(descr, components[m].description))
      {
        r = m;
      }
      else
      {
        l = m;
      }
    }
    if (components[l].description.slowCompare(descr))
      return l;
    else
      return -1;
  }
  void Archetype::update_capacity()
  {
    const auto &types = get_all_registered_types();
    if (entityCount >= totalCapacity)
    {
      totalCapacity += chunkSize;
      for (ComponentContainer &container : components)
      {
        container.allocate(chunkSize * types[container.description.typeIndex].sizeOf);
      }
    }
  }

  void Archetype::add_entity(const EntityPrefab &prefabs_list, ecs::vector<ComponentPrefab> &&overrides_list)
  {
    const auto &types = get_all_registered_types();
    uint chunk, offset;
    get_chunk_offset(entityCount, chunk, offset);
    entityCount++;
    update_capacity();
    uint j = 0;
    const auto &prefabs = prefabs_list.components;
    for (uint i = 0, n = prefabs.size(), m = overrides_list.size(); i < n; ++i)
    {
      const ComponentPrefab &component = prefabs[i];
      const auto &type = types[component.typeIndex];
      byte *memory = components[i].data[chunk] + offset * type.sizeOf;
      ECS_ASSERT(component.nameHash == components[i].description.nameHash);
      if (j < m && overrides_list[j].nameHash == component.nameHash)
      {
        type.move(memory, overrides_list[j].raw_pointer);
        j++;
      }
      else
      {
        type.copy(memory, component.raw_pointer);
      }
    }
  }

  void Archetype::destroy_entity(uint idx)
  {
    ECS_ASSERT(entityCount == 0);
    const auto &types = get_all_registered_types();
    uint chunk, offset;
    get_chunk_offset(idx, chunk, offset);
    uint chunkLast, offsetLast;
    entityCount--;
    get_chunk_offset(entityCount, chunkLast, offsetLast);

    for (ComponentContainer &container : components)
    {
      const auto &type = types[container.description.typeIndex];
      byte *component = container.data[chunk] + offset * type.sizeOf;
      type.destruct(component);
      // relocate last entity to new place
      if (idx != entityCount)
      {
        type.move(component, container.data[chunkLast] + offsetLast);
        // TODO handle eid
      }
    }
  }

  void Archetype::destroy_all_entities()
  {
    const auto &types = get_all_registered_types();

    for (ComponentContainer &container : components)
    {
      const auto &type = types[container.description.typeIndex];
      if (!type.destructor)
      {
#if ECS_OPTIMIZED_DESTRUCTION == 0
        for (auto &chunk : container.data)
          memset(chunk, ECS_CLEAR_MEM_PATTERN, chunkSize * type.sizeOf);
#endif
        continue;
      }
      uint liveEntity = entityCount;
      for (auto &chunk : container.data)
      {
        uint entitiesInChunk = chunkSize;
        if (liveEntity >= chunkSize)
        {
          liveEntity -= chunkSize;
        }
        else
        {
          entitiesInChunk = liveEntity;
          liveEntity = 0;
        }
        for (uint offset = 0; offset < entitiesInChunk; offset++)
        {
          byte *component = chunk + offset * type.sizeOf;
          type.destruct(component);
        }
      }
    }
    entityCount = 0;
  }
}