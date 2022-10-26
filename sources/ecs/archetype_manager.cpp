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
  EntityId create_entity_immediate(const EntityPrefab &prefabs_list, ecs::vector<ComponentPrefab> &&overrides_list, SizePolicy chunk_power)
  {
    if (prefabs_list.components.empty())
      return EntityId();
    uint archetype = add_archetype(prefabs_list.components, chunk_power);
    // need to validate components with async creation here.
    // also added async creation
    auto entity = get_archetype_manager().entityPool.allocate_entity();
    entity->archetype = archetype;
    get_archetype_manager().archetypes[archetype].add_entity(entity, prefabs_list, std::move(overrides_list));
    return EntityId(entity);
  }

  static void destroy_entity_immediate(EntityId eid)
  {
    uint archetype, index;
    EntityState state;
    if (eid.get_info(archetype, index, state) || state == EntityState::InDestroyQueue)
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
  }
}