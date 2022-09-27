#pragma once
#include <ecs/archetype.h>

namespace ecs
{
  struct ArchetypeManager
  {
    ecs::vector<Archetype> archetypes;
  };

  inline ArchetypeManager &get_archetype_manager()
  {
    static ArchetypeManager manager;
    return manager;
  }
}