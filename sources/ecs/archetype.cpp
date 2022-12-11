#include <ecs/archetype.h>
#include <ecs/type_annotation.h>
#include <ecs/entity_id_pool.h>
namespace ecs
{
  Archetype::Archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power)
      : chunkPower((uint)chunk_power), chunkMask((1u << (uint)chunk_power) - 1u), chunkSize(1u << (uint)chunk_power)
  {
    components.resize(descriptions.size());
    ComponentDescription eidDescr("eid", TypeIndex<EntityId>::value);

    sort_descriptions_by_names(descriptions);
    int i = 0;
    for (ComponentContainer &container : components)
    {
      container.description = std::move(descriptions[i]);
      if (container.description.fastCompare(eidDescr))
        eidContainer = &container;
      i++;
    }
    ECS_ASSERT(eidContainer);
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
    if (entityCount > totalCapacity)
    {
      totalCapacity += chunkSize;
      for (ComponentContainer &container : components)
      {
        container.allocate(chunkSize * types[container.description.typeIndex].sizeOf);
      }
    }
  }

  void Archetype::add_entity(EntityDescription *entity, const EntityPrefab &prefabs_list, ecs::vector<ComponentPrefab> &&overrides_list)
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
        ComponentPrefab &override = overrides_list[j];
        if (component.typeIndex != override.typeIndex)
        {
          ECS_ERROR("prefab \"%s\" has type missmatch for component \"%s\", it's <%s> in prefab and <%s> in override\n",
                    prefabs_list.name.c_str(),
                    component.name.c_str(),
                    type_name(component.typeIndex),
                    type_name(override.typeIndex));
          continue;
        }
        if (type.awaitConstructor)
          type.awaitConstructor(memory, override, true);
        else if (type.prefabConstructor)
          type.prefabConstructor(memory, override, true);
        else
          type.move(memory, override.get_raw_memory());
        j++;
      }
      else
      {
        if (type.awaitConstructor)
          type.awaitConstructor(memory, component, false);
        else if (type.prefabConstructor)
          type.prefabConstructor(memory, component, false);
        else
          type.copy(memory, component.get_raw_memory());
      }
    }
    entity->index = entityCount - 1;
    entity->state = EntityState::CreatedAndInited;
    EntityId *eid = (EntityId *)(eidContainer->data[chunk] + offset * sizeof(EntityId));
    new (eid) EntityId(entity);
  }

  void Archetype::destroy_entity(uint idx)
  {
    ECS_ASSERT(entityCount > 0);
    const auto &types = get_all_registered_types();
    uint chunk, offset;
    get_chunk_offset(idx, chunk, offset);
    uint chunkLast, offsetLast;
    entityCount--;
    get_chunk_offset(entityCount, chunkLast, offsetLast);

    EntityId *eid = (EntityId *)(eidContainer->data[chunkLast] + offsetLast * sizeof(EntityId));
    ((EntityDescription *)(eid->description))->index = idx;

    for (ComponentContainer &container : components)
    {
      const auto &type = types[container.description.typeIndex];
      byte *component = container.data[chunk] + offset * type.sizeOf;
      type.destruct(component);
      // relocate last entity to new place
      if (idx != entityCount)
      {
        type.move(component, container.data[chunkLast] + offsetLast * type.sizeOf);
        // TODO handle eid
      }
    }
  }

  void Archetype::destroy_all_entities(EntityPool &entity_pool)
  {
    const auto &types = get_all_registered_types();

    {
      uint liveEntity = entityCount;
      for (auto &chunk : eidContainer->data)
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
          EntityId *eid = (EntityId *)(chunk + offset * sizeof(EntityId));
          entity_pool.deallocate_entity(*eid);
        }
      }
    }

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