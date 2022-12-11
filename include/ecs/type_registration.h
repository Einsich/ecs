#pragma once
#include <ecs/type_annotation.h>
#include <ecs/component_prefab.h>

namespace ecs
{

  template <typename T>
  void copy_constuctor(void *raw_memory, const void *src)
  {
    new (raw_memory) T(*(const T *)src);
  }
  template <typename T>
  void move_constuctor(void *raw_memory, void *src)
  {
    new (raw_memory) T(std::move(*(T *)src));
  }

  template <typename T>
  void destuctor(void *raw_memory)
  {
    ((T *)raw_memory)->~T();
  }

  template <typename T>
  const TypeAnnotation *get_type_annotation()
  {
    const auto &types = get_all_registered_types();
    return TypeIndex<T>::value < (int)types.size() ? &types[TypeIndex<T>::value] : nullptr;
  }

  template <typename T, Type type = Type::DefaultType>
  void type_registration(const char *type_name,
                         PrefabConstructor prefab_ctor = nullptr,
                         ComponentAwaiter cmp_awaiter = nullptr,
                         AwaitConstructor await_ctor = nullptr)
  {
    CopyConstructor copyCtor = nullptr;
    MoveConstructor moveCtor = nullptr;
    if (cmp_awaiter == nullptr || await_ctor == nullptr)
    {
      cmp_awaiter = nullptr;
      await_ctor = nullptr;
    }

    Destructor dtor = nullptr;
    if constexpr ((type & Type::TrivialDestructor) == 0)
      dtor = destuctor<T>;
    if constexpr ((type & Type::TrivialMoveConstructor) == 0)
      moveCtor = move_constuctor<T>;
    if constexpr ((type & Type::TrivialCopyConstructor) == 0)
      copyCtor = copy_constuctor<T>;

    extern int get_next_type_index();
    extern void register_type(int, TypeAnnotation);

    ECS_ASSERT(TypeIndex<T>::value == -1);
    TypeIndex<T>::value = get_next_type_index();
    register_type(
        TypeIndex<T>::value,
        TypeAnnotation{
            type_name,
            sizeof(T),
            prefab_ctor,
            copyCtor,
            moveCtor,
            cmp_awaiter,
            await_ctor,
            dtor});
  }
  template <typename T, Type type = Type::DefaultType>
  struct RegistrationHelper
  {
    RegistrationHelper(const char *name,
                       PrefabConstructor prefab_ctor = nullptr,
                       ComponentAwaiter cmp_awaiter = nullptr,
                       AwaitConstructor await_ctor = nullptr)
    {
      type_registration<T, type>(name, prefab_ctor, cmp_awaiter, await_ctor);
    }
  };

#define ECS_TYPE_REGISTRATION(TYPE,              \
                              NAME,              \
                              TYPE_TIP,          \
                              ...)               \
  static ecs::RegistrationHelper<TYPE, TYPE_TIP> \
      __CONCAT__(registrator, __LINE__)(NAME, __VA_ARGS__);

}