#pragma once
#include <ecs/type_annotation.h>
#include <ecs/component_prefab.h>

#if !defined(USER_FUNCTION_REGISTRATION_HEADER)
#define USER_FUNCTION_REGISTRATION_HEADER <ecs/user_type_annotation.h>
#endif // MACRO

#include USER_FUNCTION_REGISTRATION_HEADER
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

  template <typename T,
            bool trivial_dtor = false,
            bool trivial_move = false,
            bool trivial_copy = false>
  void type_registration(
      const char *type_name,
      AwaitConstructor awaitCtor = AwaitConstructor{})
  {
    CopyConstructor copyCtor = nullptr;
    MoveConstructor moveCtor = nullptr;

    Destructor dtor = nullptr;
    if constexpr (!trivial_dtor)
      dtor = destuctor<T>;
    if constexpr (!trivial_move)
      moveCtor = move_constuctor<T>;
    if constexpr (!trivial_copy)
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
            copyCtor,
            moveCtor,
            awaitCtor ? awaitCtor : AwaitConstructor{},
            dtor,
            get_user_functions<T>()});
  }
  template <typename T,
            bool trivial_dtor = false,
            bool trivial_move = false,
            bool trivial_copy = false>
  struct RegistrationHelper
  {
    RegistrationHelper(const char *name, AwaitConstructor await_ctor)
    {
      type_registration<T, trivial_dtor, trivial_move, trivial_copy>(name, await_ctor);
    }
  };

#define ECS_TYPE_REGISTRATION(TYPE,                                              \
                              NAME,                                              \
                              TRIVIAL_DTOR,                                      \
                              TRIVIAL_MOVE,                                      \
                              TRIVIAL_COPY,                                      \
                              ...)                                               \
  static ecs::RegistrationHelper<TYPE, TRIVIAL_DTOR, TRIVIAL_MOVE, TRIVIAL_COPY> \
      __CONCAT__(registrator, __LINE__)(NAME, {__VA_ARGS__});

}