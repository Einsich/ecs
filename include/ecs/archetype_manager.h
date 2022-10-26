#pragma once
#include <ecs/archetype.h>
#include <ecs/entity_id_pool.h>

namespace ecs
{
  struct ArchetypeManager
  {
    ecs::vector<Archetype> archetypes;
    EntityPool entityPool;
  };

  inline ArchetypeManager &get_archetype_manager()
  {
    static ArchetypeManager manager;
    return manager;
  }
}