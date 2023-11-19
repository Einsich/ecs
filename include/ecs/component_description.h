#pragma once
#include <ecs/ecs_std.h>
#include <ecs/hash.h>

namespace ecs
{
  struct TypeFabric;
  struct ComponentDescription
  {
    ecs::string name;
    uint nameHash;
    const TypeFabric *typeDeclaration;
    ComponentDescription() = default;

    ComponentDescription(const char *name, const TypeFabric *type_declaration)
        : name(name), nameHash(ecs::hash(name)), typeDeclaration(type_declaration)
    {
      ECS_ASSERT(type_declaration != nullptr);
    }
    bool fastCompare(const ComponentDescription &other) const
    {
      return nameHash == other.nameHash && typeDeclaration == other.typeDeclaration;
    }
    bool slowCompare(const ComponentDescription &other) const
    {
      return name == other.name && typeDeclaration == other.typeDeclaration;
    }
  };
}