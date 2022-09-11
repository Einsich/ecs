#pragma once
#include <ecs/ecs_std.h>

namespace ecs
{
  template <typename T, typename U>
  struct ComponentInitializer
  {
    U value;
    ComponentInitializer(const U &value) : value(value) {}
    ComponentInitializer(U &&value) : value(std::move(value)) {}
  };

  struct ComponentPrefab
  {
    ecs::string name;
    ecs::any raw_component;
    const void *raw_pointer;

    template <typename T, typename U>
    ComponentPrefab(const char *name, const ComponentInitializer<T, U> &prefab)
        : name(name), raw_component(prefab.value), raw_pointer(get<U>())
    {
    }

    template <typename T, typename U>
    ComponentPrefab(const char *name, ComponentInitializer<T, U> &&prefab)
        : name(name), raw_component(std::move(prefab.value)), raw_pointer(get<U>())
    {
    }

    template <typename T>
    ComponentPrefab(const char *name, const T &prefab)
        : name(name), raw_component(prefab), raw_pointer(get<T>())
    {
    }

    template <typename T>
    ComponentPrefab(const char *name, T &&prefab)
        : name(name), raw_component(std::move(prefab)), raw_pointer(get<T>())
    {
    }
    template <typename T>
    const T *get() const
    {
      return raw_component._Cast<T>();
    }

    ~ComponentPrefab()
    {
      raw_component.~any();
    }
  };
}