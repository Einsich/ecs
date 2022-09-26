#pragma once
#include <ecs/ecs_std.h>
#include <ecs/hash.h>

namespace ecs
{
  struct ComponentDescription
  {
    ecs::string name;
    uint nameHash;
    uint typeIndex;
    ComponentDescription() = default;

    ComponentDescription(const char *name, int type_index)
        : name(name), nameHash(ecs::hash(name)), typeIndex(type_index)
    {
      ECS_ASSERT(type_index >= 0);
    }
  };
}