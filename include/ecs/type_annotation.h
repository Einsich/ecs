#pragma once
#include <ecs/ecs_std.h>

#if !defined(USER_FUNCTION_HEADER)
#define USER_FUNCTION_HEADER <ecs/user_type_annotation.h>
#endif // MACRO

#include USER_FUNCTION_HEADER

namespace ecs
{
  enum Type
  {
    DefaultType = 0,
    TrivialCopyConstructor = 1 << 0,
    TrivialMoveConstructor = 1 << 1,
    TrivialDestructor = 1 << 2,
    PODType = TrivialCopyConstructor | TrivialMoveConstructor | TrivialDestructor,
  };
  struct ComponentPrefab;
  struct TypeFabric
  {
    const ecs::string name;
    TypeFabric *nextFabric = nullptr;
    const size_t sizeOf = 0;
    const bool trivialCopy = false;
    const bool trivialMove = false;
    const bool trivialDestruction = false;
    const bool hasPrefabCtor = false;
    const bool hasAwaiter = false;

    static inline TypeFabric *linked_list_head = nullptr;

    TypeFabric(const char *name, Type type, bool has_prefab_ctor, bool has_awaiter, size_t size);

    virtual void copy_constructor(void *raw_memory, const void *source) const = 0;
    virtual void move_constructor(void *raw_memory, void *source) const = 0;
    virtual void prefab_constructor(void *raw_memory, const ComponentPrefab &prefab, bool is_rvalue) const = 0;
    virtual void destructor(void *memory) const = 0;
    virtual bool component_awaiter(const ComponentPrefab &prefab) const = 0;
    virtual void await_contructor(void *raw_memory, const ComponentPrefab &prefab, bool is_rvalue) const = 0;
  };

  template <typename T>
  struct TypeIndex
  {
    inline static const TypeFabric *value = nullptr;
  };
}