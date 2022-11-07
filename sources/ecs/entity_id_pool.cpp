#include <ecs/entity_id_pool.h>

namespace ecs
{
  EntityPool::~EntityPool()
  {
    for_each<true>(EntityState::Destoyed, [](EntityDescription &)
                   { ECS_ASSERT(0); });
    for (auto batch : entities)
      delete[] batch;
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
    EntityDescription *entity = entities.back() + (idx & entityBinMask);
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
