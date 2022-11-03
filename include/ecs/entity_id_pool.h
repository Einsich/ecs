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
    ~EntityPool();

    void allocate_more_entity();

    void ECS_INLINE get_chunk_offset(uint idx, uint &chunk, uint &offset)
    {
      chunk = idx >> entityBinPower;
      offset = idx & entityBinMask;
    }

    EntityDescription *allocate_entity();

    void deallocate_entity(EntityDescription *entity);

    void deallocate_entity(EntityId eid);

    void add_entity_to_destroy_queue(EntityId eid);

    template <bool invert_filter = false, typename Callable>
    void for_each(EntityState filter, Callable callback)
    {
      uint last = used;
      for (auto batch : entities)
      {
        for (uint i = 0, n = std::min(entityBinSize, last); i < n; i++)
        {
          if ((batch[i].state == filter) ^ invert_filter)
            callback(batch[i]);
        }
        last -= entityBinSize;
      }
    }
  };
}