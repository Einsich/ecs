#pragma once
#include <ecs/ecs_std.h>

#if !defined(USER_FUNCTION_HEADER)
#define USER_FUNCTION_HEADER <ecs/user_type_annotation.h>
#endif // MACRO

#include USER_FUNCTION_HEADER

namespace ecs
{
  using DefaultConstructor = void (*)(void *);
  using CopyConstructor = void (*)(void *, const void *);
  using MoveConstructor = void (*)(void *, void *);
  using Destructor = void (*)(void *);

  struct TypeAnnotation
  {
    const ecs::string name;
    const DefaultConstructor defaultConstructor = nullptr;
    const CopyConstructor copyConstructor = nullptr;
    const MoveConstructor moveConstructor = nullptr;
    const Destructor destructor = nullptr;
    const UserFunctions userFunctions;
  };
  int type_name_to_index(const char *name);
  const ecs::vector<TypeAnnotation> &get_all_registered_types();
}