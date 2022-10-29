#include <ecs/entity_id_pool.h>

namespace ecs
{
  EntityPool::~EntityPool()
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

  void EntityPool::allocate_more_entity()
  {
    allocated += entityBinSize;
    entities.push_back(new EntityDescription[entityBinSize]);
  }

  EntityDescription *EntityPool::allocate_entity()
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

  void EntityPool::deallocate_entity(EntityDescription *entity)
  {
    ECS_ASSERT(entity->valid_for_destroy());
    entity->archetype = -1;
    entity->index = -1;
    entity->state = EntityState::Destoyed;
    active--;
  }

  void EntityPool::deallocate_entity(EntityId eid)
  {
    ECS_ASSERT(eid.description);
    deallocate_entity((EntityDescription *)eid.description);
  }

  void EntityPool::add_entity_to_destroy_queue(EntityId eid)
  {
    ((EntityDescription *)eid.description)->state = EntityState::InDestroyQueue;
    entitiesToDestroy.push(eid);
  }
}
