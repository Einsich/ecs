#include <ecs/archetype_manager.h>
#include <ecs/query_manager.h>

namespace ecs
{
  template <typename Description>
  static int find_archetype(const ecs::vector<Description> &descriptions, const ecs::vector<ecs::Archetype> &archetypes)
  {
    int result = 0;
    for (const auto &archetype : archetypes)
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

  uint add_archetype(
      ecs::vector<ecs::Archetype> &archetypes,
      const ecs::vector<ComponentPrefab> &prefabs,
      SizePolicy chunk_power)
  {
    int idx = find_archetype(prefabs, archetypes);
    if (idx < 0)
    {
      ecs::vector<ComponentDescription> descriptions(prefabs.size());
      for (uint i = 0, n = prefabs.size(); i < n; i++)
        descriptions[i] = prefabs[i];
      idx = archetypes.size();
      archetypes.emplace_back(std::move(descriptions), chunk_power);
      get_query_manager().addArchetypeToCache(idx);
    }
    else
    {
      if (archetypes[idx].chunkPower != (uint)chunk_power)
      {
        ECS_ERROR("missmatch in chunk sizes");
      }
    }
    return idx;
  }

  static archetype_id get_archetype_id(const EntityPrefab &prefab, prefab_id id, ecs::ArchetypeManager &manager)
  {
    archetype_id archetype = manager.get_archetype_for_prefab(id);
    if (archetype == invalidArchetypeId)
    {
      archetype = add_archetype(manager.archetypes, prefab.components, prefab.chunkPower);
      manager.add_prefab_archetype_cache(id, archetype);
    }
    return archetype;
  }

  EntityId create_entity_immediate(prefab_id id, ecs::vector<ComponentPrefab> &&overrides_list)
  {
    if (id == invalidPrefabId)
      return EntityId();
    const EntityPrefab &prefab = get_prefab(id);
    archetype_id archetype = get_archetype_id(prefab, id, get_archetype_manager());
    auto entity = get_archetype_manager().entityPool.allocate_entity();
    entity->archetype = archetype;
    get_archetype_manager().archetypes[archetype].add_entity(entity, prefab, std::move(overrides_list));
    return EntityId(entity);
  }

  EntityId create_entity(prefab_id id, ecs::vector<ComponentPrefab> &&overrides_list)
  {
    if (id == invalidPrefabId)
      return EntityId();

    auto entity = get_archetype_manager().entityPool.allocate_entity();
    get_archetype_manager().defferedEntityCreation.push(
        ArchetypeManager::DefferedEntityCreation{
            std::move(overrides_list), id, entity});

    return EntityId(entity);
  }

  static void create_deffered_entities()
  {
    auto &mgr = get_archetype_manager();
    for (uint i = 0, n = mgr.defferedEntityCreation.size(); i < n; i++)
    {
      auto &defferedEntity = mgr.defferedEntityCreation.front();
      EntityDescription *entity = defferedEntity.entity;
      const EntityPrefab &prefab = get_prefab(defferedEntity.prefab);
      archetype_id archetype = get_archetype_id(prefab, defferedEntity.prefab, mgr);

      entity->archetype = archetype;
      get_archetype_manager().archetypes[archetype].add_entity(entity, prefab, std::move(defferedEntity.overrides_list));

      mgr.defferedEntityCreation.pop();
    }
  }

  static void destroy_entity_immediate(EntityId eid)
  {
    uint archetype, index;
    EntityState state;
    if (eid.get_info(archetype, index, state) && (state == EntityState::CreatedAndInited || state == EntityState::InDestroyQueue))
    {
      get_archetype_manager().archetypes[archetype].destroy_entity(index);
      get_archetype_manager().entityPool.deallocate_entity(eid);
    }
  }

  void destroy_entity(EntityId eid)
  {
    uint archetype, index;
    EntityState state;
    if (eid.get_info(archetype, index, state) || state == EntityState::CreatedNotInited)
    {
      get_archetype_manager().entityPool.add_entity_to_destroy_queue(eid);
    }
    // may be need to destroy immediate if entity not inited yet
    // if (state == EntityState::CreatedNotInited)
    //   destroy_entity_immediate(eid);
  }

  static void destroy_queued_entities()
  {
    auto &destroyQueue = get_archetype_manager().entityPool.entitiesToDestroy;
    for (uint i = 0, n = destroyQueue.size(); i < n; i++)
    {
      EntityId eid = destroyQueue.front();
      destroyQueue.pop();
      destroy_entity_immediate(eid);
    }
  }

  void destroy_all_entities()
  {
    for (auto &archetype : get_archetype_manager().archetypes)
      archetype.destroy_all_entities(get_archetype_manager().entityPool);
  }

  void update_archetype_manager()
  {
    destroy_queued_entities();
    create_deffered_entities();
  }
}