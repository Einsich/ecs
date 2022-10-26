#pragma once
#include <ecs/ecs_std.h>

#if !defined(USER_FUNCTION_HEADER)
#define USER_FUNCTION_HEADER <ecs/user_type_annotation.h>
#endif // MACRO

#include USER_FUNCTION_HEADER

namespace ecs
{
  struct ComponentPrefab;
  using DefaultConstructor = void (*)(void *);
  using CopyConstructor = void (*)(void *, const void *);
  using MoveConstructor = void (*)(void *, void *);
  using SpecialConstructor = void (*)(void *, const ComponentPrefab &);
  using Destructor = void (*)(void *);

  struct TypeAnnotation
  {
    const ecs::string name;
    const uint sizeOf;
    const DefaultConstructor defaultConstructor = nullptr;
    const CopyConstructor copyConstructor = nullptr;
    const MoveConstructor moveConstructor = nullptr;
    const SpecialConstructor specialConstructor = nullptr;
    const Destructor destructor = nullptr;
    const UserFunctions userFunctions;

    void ECS_INLINE construct(void *data) const
    {
      if (defaultConstructor)
        defaultConstructor(data);
      else
        memset(data, 0, sizeOf);
    }
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