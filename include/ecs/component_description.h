#pragma once
#include <ecs/ecs_std.h>
#include <ecs/hash.h>

namespace ecs
{
  struct Hash
  {
private:
#ifndef NDEBUG
    ecs::string name;
#endif
public:
    uint hash;
    constexpr Hash() = default;
    constexpr Hash(Hash &&) = default;
    constexpr Hash(const Hash &) = default;
    constexpr Hash(const char *c_name):
#ifndef NDEBUG
      name(c_name),
#endif
      hash(ecs::hash(c_name)){ (void)c_name; }

    constexpr Hash(uint ecs_hash, const char *c_name):
#ifndef NDEBUG
      name(c_name),
#endif
      hash(ecs_hash){ (void)c_name; }

    const char *c_str() const
    {
#ifndef NDEBUG
      return name.c_str();
#else
      return "<stub name>";
#endif
    }
  };


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
      // ECS_ASSERT(type_declaration != nullptr);
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