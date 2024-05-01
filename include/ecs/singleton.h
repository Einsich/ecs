#pragma once
#include <ecs/ecs_std.h>
#include <ecs/type_annotation.h>

namespace ecs
{
  struct Singleton
  {
  };


  using SingletonContructor = void (*)(void *);
  using SingletonDestructor = void (*)(void *);

  template <typename T>
  void singleton_destuctor(void *raw_memory)
  {
    ((T *)raw_memory)->~T();
  }

  template <typename T>
  void singleton_contructor(void *raw_memory)
  {
    new (raw_memory) T();
  }

  struct SingletonTypeFabric
  {
    ecs::string name;
    size_t sizeOf;
    const SingletonContructor contructor;
    const SingletonDestructor destructor;

    SingletonTypeFabric *nextFabric = nullptr;
    static inline SingletonTypeFabric *linked_list_head = nullptr;

    SingletonTypeFabric(const char *name, size_t sizeOf, SingletonContructor ctor, SingletonDestructor dtor):
      name(name),
      sizeOf(sizeOf),
      contructor(ctor),
      destructor(dtor)
    {
      if (linked_list_head != nullptr)
        linked_list_head->nextFabric = this;
      linked_list_head = this;
    }

  };

  template <typename T>
  struct SingletonIndex
  {
    inline static const SingletonTypeFabric *value = nullptr;
  };

  template <typename T>
  struct SingletonTypeRegistration : public SingletonTypeFabric
  {
    SingletonTypeRegistration(const char *name):
    SingletonTypeFabric(name, sizeof(T), singleton_contructor<T>, singleton_destuctor<T>)
    {
      ECS_ASSERT(SingletonIndex<T>::value == nullptr);
      SingletonIndex<T>::value = this;
      extern bool hasNewSingletonTypes;
      hasNewSingletonTypes = true;
    }
  };


  void preallocate_singletones();
  void init_singletones();
  void destroy_sinletons();

  void *get_singleton(intptr_t idx);

  template <typename T>
  T *get_singleton()
  {
    return (T *)get_singleton((intptr_t)SingletonIndex<std::remove_cvref_t<T>>::value);
  }

  template <typename T>
  constexpr bool is_singleton()
  {
    return std::is_base_of_v<ecs::Singleton, T>;
  }
  template <typename T>
  struct is_singleton_type
  {
    static constexpr bool value = is_singleton<T>();
  };

}
#define ECS_REGISTER_SINGLETON(T) \
  static ecs::SingletonTypeRegistration<T> __CONCAT__(singletonRegistrator, __COUNTER__)(#T);