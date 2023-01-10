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
    const bool trivialCopy = false;
    const bool trivialMove = false;
    const bool trivialDestruction = false;
    const bool hasPrefabCtor = false;
    const bool hasAwaiter = false;
    TypeFabric(Type type, bool has_prefab_ctor, bool has_awaiter):
      trivialCopy(type&TrivialCopyConstructor),
      trivialMove(type&TrivialMoveConstructor),
      trivialDestruction(type&TrivialDestructor),
      hasPrefabCtor(has_prefab_ctor),
      hasAwaiter(has_awaiter)
    {}

    virtual void copy_constructor(void *raw_memory, const void *source) const = 0;
    virtual void move_constructor(void *raw_memory, void *source) const = 0;
    virtual void prefab_constructor(void *raw_memory, const ComponentPrefab &prefab, bool is_rvalue) const = 0;
    virtual void destructor(void *memory) const = 0;
    virtual bool component_awaiter(const ComponentPrefab &prefab) const = 0;
    virtual void await_contructor(void *raw_memory, const ComponentPrefab &prefab, bool is_rvalue) const = 0;
  };

  struct TypeAnnotation
  {
    const ecs::string name;
    const uint sizeOf;
    const TypeFabric *typeFabric = nullptr;

    void ECS_INLINE copy(void *dst, const void *src) const
    {
      if (typeFabric->trivialCopy)
        memcpy(dst, src, sizeOf);
      else
        typeFabric->copy_constructor(dst, src);
    }
    void ECS_INLINE move(void *dst, void *src) const
    {
      if (typeFabric->trivialMove)
      {
        memcpy(dst, src, sizeOf);
        memset(src, 0, sizeOf);
      }
      else
        typeFabric->move_constructor(dst, src);
    }
    void ECS_INLINE destruct(void *dst) const
    {
      if (!typeFabric->trivialDestruction)
        typeFabric->destructor(dst);
#if ECS_OPTIMIZED_DESTRUCTION == 0
      else
        memset(dst, ECS_CLEAR_MEM_PATTERN, sizeOf);
#endif
    }
  };
  template <typename T>
  struct TypeIndex
  {
    inline static int value = -1;
  };
  int type_name_to_index(const char *name);
  const char *type_name(uint type_idx);
  const ecs::vector<TypeAnnotation> &get_all_registered_types();
}