#include <ecs/archetype_manager.h>
#include <ecs/query_manager.h>
#include <ecs/type_annotation.h>
#include <ecs/base_events.h>
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
          goto next_iteration;
      return result - 1;
    next_iteration:
      continue;
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

  static void create_entity(
      Archetype &archetype,
      EntityDescription *entity,
      const EntityPrefab &prefab,
      ecs::vector<ComponentPrefab> &&overrides_list)
  {
    archetype.add_entity(entity, prefab, std::move(overrides_list));
    get_query_manager().send_event_immediate(entity, ecs::OnEntityCreated(), ecs::EventIndex<ecs::OnEntityCreated>::value);
  }

  EntityId create_entity_immediate(prefab_id id, ecs::vector<ComponentPrefab> &&overrides_list)
  {
    if (id == invalidPrefabId)
      return EntityId();
    const EntityPrefab &prefab = get_prefab(id);
    if (prefab.requireAwaitCreation)
    {
      ECS_ERROR("prefab %s requier await creation, use create_entity, instead create_entity_immediate", prefab.name.c_str());
      return EntityId();
    }
    auto &mgr = get_archetype_manager();
    archetype_id archetype = get_archetype_id(prefab, id, mgr);
    auto entity = mgr.entityPool.allocate_entity();
    entity->archetype = archetype;
    sort_prefabs_by_names(overrides_list);
    create_entity(mgr.archetypes[archetype], entity, prefab, std::move(overrides_list));
    return EntityId(entity);
  }

  using AwaitPrefab = ArchetypeManager::AwaitEntityCreation::AwaitPrefab;
  static ecs::vector<AwaitPrefab> get_cache(
      const ecs::vector<ComponentPrefab> &prefabs,
      const ecs::vector<ComponentPrefab> &overrides)
  {
    const auto &types = get_all_registered_types();
    ecs::vector<AwaitPrefab> cache;
    for (uint i = 0, j = 0, n = prefabs.size(), m = overrides.size(); i < n; ++i)
    {
      const ComponentPrefab &component = prefabs[i];
      uint k = i;
      bool inPrefab = true;
      if (j < m && overrides[j].nameHash == component.nameHash)
      {
        // ECS_ASSERT(component.typeIndex == overrides[j].typeIndex);
        k = j;
        inPrefab = false;
        j++;
      }
      if (types[component.typeIndex].componentAwaiter)
        cache.emplace_back(AwaitPrefab{k, inPrefab});
    }
    return cache;
  }

  bool ArchetypeManager::AwaitEntityCreation::ready() const
  {
    const auto &types = get_all_registered_types();
    const EntityPrefab &prefab = get_prefab(prefabId);
    for (const AwaitPrefab &awaitComp : awaitCache)
    {
      const auto &component = awaitComp.inPrefab ? prefab.components[awaitComp.idx] : overrides_list[awaitComp.idx];
      if (!types[component.typeIndex].componentAwaiter(component))
        return false;
    }
    return true;
  }

  EntityId create_entity(prefab_id id, ecs::vector<ComponentPrefab> &&overrides_list)
  {
    if (id == invalidPrefabId)
      return EntityId();
    auto &mgr = get_archetype_manager();
    const EntityPrefab &prefab = get_prefab(id);

    auto entity = mgr.entityPool.allocate_entity();
    sort_prefabs_by_names(overrides_list);
    if (!prefab.requireAwaitCreation)
    {
      mgr.defferedEntityCreation.push(
          ArchetypeManager::DefferedEntityCreation{
              std::move(overrides_list), id, entity});
    }
    else
    {
      ecs::vector<AwaitPrefab> awaitPrefabs = get_cache(prefab.components, overrides_list);

      mgr.awaitEntityCreation.emplace_back(
          ArchetypeManager::AwaitEntityCreation{
              {std::move(overrides_list),
               id,
               entity},
              std::move(awaitPrefabs)});
    }

    return EntityId(entity);
  }

  static void create_deffered_entities()
  {
    auto &mgr = get_archetype_manager();
    for (uint i = 0, n = mgr.defferedEntityCreation.size(); i < n; i++)
    {
      auto &defferedEntity = mgr.defferedEntityCreation.front();
      EntityDescription *entity = defferedEntity.entity;
      const EntityPrefab &prefab = get_prefab(defferedEntity.prefabId);
      archetype_id archetype = get_archetype_id(prefab, defferedEntity.prefabId, mgr);

      entity->archetype = archetype;
      create_entity(mgr.archetypes[archetype], entity, prefab, std::move(defferedEntity.overrides_list));

      mgr.defferedEntityCreation.pop();
    }

    for (uint i = 0, n = mgr.awaitEntityCreation.size(); i < n; i++)
    {
      auto &awaitedEntity = mgr.awaitEntityCreation[i];
      if (!awaitedEntity.ready())
        continue;

      EntityDescription *entity = awaitedEntity.entity;
      const EntityPrefab &prefab = get_prefab(awaitedEntity.prefabId);
      archetype_id archetype = get_archetype_id(prefab, awaitedEntity.prefabId, mgr);

      entity->archetype = archetype;
      create_entity(mgr.archetypes[archetype], entity, prefab, std::move(awaitedEntity.overrides_list));

      awaitedEntity.entity = nullptr; // mark awaitEntity dirty;
    }
    if (!mgr.awaitEntityCreation.empty()) // This algorithm can have bugs!!!
    {
      int i = 0;
      int j = mgr.awaitEntityCreation.size() - 1;
      for (i = 0; i <= j; i++)
      {
        if (!mgr.awaitEntityCreation[i].entity)
        {
          while (j >= 0 && !mgr.awaitEntityCreation[j].entity)
            j--;
          if (j < 0 || j <= i)
            break;
          std::swap(mgr.awaitEntityCreation[i], mgr.awaitEntityCreation[j]);
          j--;
        }
      }
      mgr.awaitEntityCreation.resize(j + 1);
    }
  }

  EntityId create_entity(const char *prefab_name, ecs::vector<ComponentPrefab> &&overrides_list)
  {
    ecs::prefab_id id = ecs::get_prefab_id(prefab_name);
    return ecs::create_entity(id, std::move(overrides_list));
  }

  static void destroy_entity_immediate(EntityId eid)
  {
    auto &mgr = get_archetype_manager();
    uint archetype, index;
    EntityState state;
    if (eid.get_info(archetype, index, state) && (state == EntityState::CreatedAndInited || state == EntityState::InDestroyQueue))
    {
      get_query_manager().send_event_immediate(eid, ecs::OnEntityDestroyed(), ecs::EventIndex<ecs::OnEntityDestroyed>::value);
      get_query_manager().send_event_immediate(eid, ecs::OnEntityTerminated(), ecs::EventIndex<ecs::OnEntityTerminated>::value);
      mgr.archetypes[archetype].destroy_entity(index);
      mgr.entityPool.deallocate_entity(eid);
    }
  }

  void destroy_entity(EntityId eid)
  {
    auto &mgr = get_archetype_manager();
    uint archetype, index;
    EntityState state;
    if (eid.get_info(archetype, index, state) || state == EntityState::CreatedNotInited)
    {
      mgr.entityPool.add_entity_to_destroy_queue(eid);
    }
    // may be need to destroy immediate if entity not inited yet
    // if (state == EntityState::CreatedNotInited)
    //   destroy_entity_immediate(eid);
  }

  static void destroy_queued_entities()
  {
    auto &mgr = get_archetype_manager();
    auto &destroyQueue = mgr.entityPool.entitiesToDestroy;
    for (uint i = 0, n = destroyQueue.size(); i < n; i++)
    {
      EntityId eid = destroyQueue.front();
      destroyQueue.pop();
      destroy_entity_immediate(eid);
    }
  }

  void destroy_all_entities()
  {
    destroy_queued_entities();
    auto &mgr = get_archetype_manager();
    auto &qMgr = get_query_manager();
    mgr.entityPool.for_each(ecs::EntityState::CreatedAndInited, [&](ecs::EntityDescription &entity)
                            { qMgr.send_event_immediate(&entity, ecs::OnEntityTerminated(), ecs::EventIndex<ecs::OnEntityTerminated>::value); });

    for (auto &archetype : mgr.archetypes)
      archetype.destroy_all_entities(mgr.entityPool);

    while (!mgr.defferedEntityCreation.empty())
    {
      mgr.entityPool.deallocate_entity(mgr.defferedEntityCreation.front().entity);
      mgr.defferedEntityCreation.pop();
    }
    for (auto &awaitEntity : mgr.awaitEntityCreation)
    {
      mgr.entityPool.deallocate_entity(awaitEntity.entity);
    }
    mgr.awaitEntityCreation.clear();
  }

  void update_archetype_manager()
  {
    destroy_queued_entities();
    create_deffered_entities();
  }
}