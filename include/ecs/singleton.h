#pragma once
#include <ecs/ecs_std.h>

namespace ecs
{
  struct Singleton
  {
  };

  struct PersistentSingleton
  {
  };

  template <typename T>
  struct SingletonIndex
  {
    inline static int value = -1;
  };
  using SingletonContructor = void (*)(void *);
  using SingletonDestructor = void (*)(void *);
  struct SingletonDescription
  {
    ecs::string name;
    const SingletonContructor contructor;
    const SingletonDestructor destructor;
    const uint sizeOf;
    bool persistent;
  };

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
  template <typename T>
  void register_singleton(const char *name)
  {
    bool persistent = std::is_base_of_v<ecs::PersistentSingleton, T>;

    extern int get_next_singleton_index();
    extern void register_singleton(int idx, SingletonDescription descr);
    extern bool hasNewSingletonTypes;

    hasNewSingletonTypes = true;
    ECS_ASSERT(SingletonIndex<T>::value == -1);
    SingletonIndex<T>::value = get_next_singleton_index();
    register_singleton(SingletonIndex<T>::value,
                       SingletonDescription{name, singleton_contructor<T>, singleton_destuctor<T>, sizeof(T), persistent});
  }

  template <typename T>
  struct SingletonRegister
  {
    SingletonRegister(const char *name, SingletonContructor constructor = nullptr)
    {
      register_singleton<T>(name, constructor);
    }
  };

  void preallocate_singletones();
  void init_singletones();
  void destroy_sinletons();

  void *get_singleton(uint idx);

  template <typename T>
  T *get_singleton()
  {
    return (T *)get_singleton(SingletonIndex<std::remove_cvref_t<T>>::value);
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
  static ecs::SingletonRegister<T> __CONCAT__(singletonRegistrator, __COUNTER__)(#T);