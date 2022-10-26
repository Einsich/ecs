#pragma once
#include <ecs/entity_id.h>

namespace ecs
{
  constexpr uint entityBinPower = 10;
  constexpr uint entityBinSize = 1u << entityBinPower;
  constexpr uint entityBinMask = entityBinSize - 1;

  struct EntityPool
  {
    ecs::vector<EntityDescription *> entities;
    ecs::queue<EntityId> entitiesToDestroy;

    uint allocated = 0;
    uint used = 0;
    uint active = 0;
    ~EntityPool()
    {
      uint last = used;

      for (auto batch : entities)
      {
        for (uint i = 0, n = std::min(entityBinSize, last); i < n; i++)
        {
          ECS_ASSERT(batch[i].state == EntityState::Destoyed);
        }
        last -= entityBinSize;

        delete[] batch;
      }
    }

    void allocate_more_entity()
    {
      allocated += entityBinSize;
      entities.push_back(new EntityDescription[entityBinSize]);
    }

    void ECS_INLINE get_chunk_offset(uint idx, uint &chunk, uint &offset)
    {
      chunk = idx >> entityBinPower;
      offset = idx & entityBinMask;
    }

    EntityDescription *allocate_entity()
    {
      if (used == allocated)
        allocate_more_entity();
      uint idx = used;
      used++;
      active++;
      EntityDescription *entity = entities.back() + idx;
      entity->state = EntityState::CreatedNotInited;
      return entity;
    }

    void deallocate_entity(EntityDescription *entity)
    {
      ECS_ASSERT(entity->valid_for_destroy());
      entity->archetype = -1;
      entity->index = -1;
      entity->state = EntityState::Destoyed;
      active--;
    }

    void deallocate_entity(EntityId eid)
    {
      ECS_ASSERT(eid.description);
      deallocate_entity((EntityDescription *)eid.description);
    }

    void add_entity_to_destroy_queue(EntityId eid)
    {
      ((EntityDescription *)eid.description)->state = EntityState::InDestroyQueue;
      entitiesToDestroy.push(eid);
    }
  };
}