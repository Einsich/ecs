#include <ecs/archetype.h>
#include <ecs/type_annotation.h>
#include <ecs/entity_id_pool.h>
namespace ecs
{
  Archetype::Archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power)
      : chunkPower((uint)chunk_power), chunkMask((1u << (uint)chunk_power) - 1u), chunkSize(1u << (uint)chunk_power)
  {
    components.resize(descriptions.size());
    ComponentDescription eidDescr("eid", TypeIndex<EntityId>::fabric);

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
    if (entityCount > totalCapacity)
    {
      totalCapacity += chunkSize;
      for (ComponentContainer &container : components)
      {
        container.allocate(chunkSize * container.description.typeDeclaration->sizeOf);
      }
    }
  }

  void Archetype::add_entity(EntityDescription *entity, const EntityPrefab &prefabs_list, ecs::vector<ComponentPrefab> &&overrides_list)
  {
    uint chunk, offset;
    get_chunk_offset(entityCount, chunk, offset);
    entityCount++;
    update_capacity();
    uint j = 0;
    const auto &prefabs = prefabs_list.components;
    for (uint i = 0, n = prefabs.size(), m = overrides_list.size(); i < n; ++i)
    {
      const ComponentPrefab &component = prefabs[i];
      const auto &fabric = *component.typeDeclaration;
      byte *memory = components[i].data[chunk] + offset * fabric.sizeOf;
      ECS_ASSERT(component.nameHash == components[i].description.nameHash);
      if (j < m && overrides_list[j].nameHash == component.nameHash)
      {
        ComponentPrefab &override = overrides_list[j];
        if (component.typeDeclaration != override.typeDeclaration)
        {
          ECS_ERROR("prefab \"%s\" has type missmatch for component \"%s\", it's <%s> in prefab and <%s> in override",
                    prefabs_list.name.c_str(),
                    component.name.c_str(),
                    fabric.name.c_str(), override.typeDeclaration->name.c_str());
          continue;
        }
        if (fabric.hasAwaiter)
          fabric.await_contructor(memory, override, true);
        else if (fabric.hasPrefabCtor)
          fabric.prefab_constructor(memory, override, true);
        else
          fabric.move_constructor(memory, override.get_raw_memory());
        j++;
      }
      else
      {
        if (fabric.hasAwaiter)
          fabric.await_contructor(memory, component, true);
        else if (fabric.hasPrefabCtor)
          fabric.prefab_constructor(memory, component, true);
        else
          fabric.copy_constructor(memory, component.get_raw_memory());
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
    uint chunk, offset;
    get_chunk_offset(idx, chunk, offset);
    uint chunkLast, offsetLast;
    entityCount--;
    get_chunk_offset(entityCount, chunkLast, offsetLast);

    EntityId *eid = (EntityId *)(eidContainer->data[chunkLast] + offsetLast * sizeof(EntityId));
    ((EntityDescription *)(eid->description))->index = idx;

    for (ComponentContainer &container : components)
    {
      const auto &fabric = *container.description.typeDeclaration;
      byte *component = container.data[chunk] + offset * fabric.sizeOf;
      fabric.destructor(component);
      // relocate last entity to new place
      if (idx != entityCount)
      {
        fabric.move_constructor(component, container.data[chunkLast] + offsetLast * fabric.sizeOf);
        // TODO handle eid
      }
    }
  }

  void Archetype::destroy_all_entities(EntityPool &entity_pool)
  {
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
      const auto &fabric = *container.description.typeDeclaration;
      if (fabric.trivialDestruction)
      {
#if ECS_OPTIMIZED_DESTRUCTION == 0
        for (auto &chunk : container.data)
          memset(chunk, ECS_CLEAR_MEM_PATTERN, chunkSize * fabric.sizeOf);
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
          byte *component = chunk + offset *fabric.sizeOf;
          fabric.destructor(component);
        }
      }
    }
    entityCount = 0;
  }
}