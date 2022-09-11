#pragma once
#include <ecs/type_annotation.h>
#include <ecs/component_prefab.h>

namespace ecs
{

  template <typename T>
  void default_constuctor(void *raw_memory)
  {
    new (raw_memory) T();
  }
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
  void special_constuctor(void *raw_memory, const ComponentPrefab &prefab)
  {
    new (raw_memory) T(prefab);
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

  template <typename T,
            bool trivial_ctor = false,
            bool trivial_dtor = false,
            bool trivial_move = false,
            bool trivial_copy = false,
            bool special_ctor = false>
  void type_registration(const char *type_name, const UserFunctions user_functions = UserFunctions{})
  {
    DefaultConstructor ctor = nullptr;
    CopyConstructor copyCtor = nullptr;
    MoveConstructor moveCtor = nullptr;
    SpecialConstructor specialCtor = nullptr;
    Destructor dtor = nullptr;
    if constexpr (!trivial_ctor)
      ctor = default_constuctor<T>;
    if constexpr (!trivial_dtor)
      dtor = destuctor<T>;
    if constexpr (!trivial_move)
      moveCtor = move_constuctor<T>;
    if constexpr (!trivial_copy)
      copyCtor = copy_constuctor<T>;
    if constexpr (special_ctor)
      specialCtor = special_constuctor<T>;

    extern int get_next_type_index();
    extern void register_type(int, TypeAnnotation);

    TypeIndex<T>::value = get_next_type_index();
    register_type(TypeIndex<T>::value, TypeAnnotation{type_name, ctor, copyCtor, moveCtor, specialCtor, dtor, user_functions});
  }
  template <typename T,
            bool trivial_ctor = false,
            bool trivial_dtor = false,
            bool trivial_move = false,
            bool trivial_copy = false,
            bool special_ctor = false>
  struct RegistrationHelper
  {
    RegistrationHelper(const char *name, UserFunctions user_functions)
    {
      type_registration<T, trivial_ctor, trivial_dtor, trivial_move, trivial_copy, special_ctor>(name, user_functions);
    }
  };

#define ECS_TYPE_REGISTRATION(TYPE,                                                                                 \
                              NAME,                                                                                 \
                              TRIVIAL_CTOR,                                                                         \
                              TRIVIAL_DTOR,                                                                         \
                              TRIVIAL_MOVE,                                                                         \
                              TRIVIAL_COPY,                                                                         \
                              SPECIAL_CONSTRUCTOR,                                                                  \
                              USER_FUNCTIONS)                                                                       \
  static ecs::RegistrationHelper<TYPE, TRIVIAL_CTOR, TRIVIAL_DTOR, TRIVIAL_MOVE, TRIVIAL_COPY, SPECIAL_CONSTRUCTOR> \
      __CONCAT__(registrator, __LINE__)(NAME, USER_FUNCTIONS);

}