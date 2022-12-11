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
  using PrefabConstructor = void (*)(void *, const ComponentPrefab &);
  using CopyConstructor = void (*)(void *, const void *);
  using MoveConstructor = void (*)(void *, void *);
  using Destructor = void (*)(void *);
  using ComponentAwaiter = bool (*)(const ComponentPrefab &);
  using AwaitConstructor = void (*)(void *, const ComponentPrefab &);

  struct TypeAnnotation
  {
    const ecs::string name;
    const uint sizeOf;
    const PrefabConstructor prefabConstructor = nullptr;
    const CopyConstructor copyConstructor = nullptr;
    const MoveConstructor moveConstructor = nullptr;
    const ComponentAwaiter componentAwaiter = nullptr;
    const AwaitConstructor awaitConstructor = nullptr;
    const Destructor destructor = nullptr;
    const UserFunctions userFunctions;

    void ECS_INLINE copy(void *dst, const void *src) const
    {
      if (copyConstructor)
        copyConstructor(dst, src);
      else
        memcpy(dst, src, sizeOf);
    }
    void ECS_INLINE move(void *dst, void *src) const
    {
      if (moveConstructor)
        moveConstructor(dst, src);
      else
      {
        memcpy(dst, src, sizeOf);
        memset(src, 0, sizeOf);
      }
    }
    void ECS_INLINE destruct(void *dst) const
    {
      if (destructor)
        destructor(dst);
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