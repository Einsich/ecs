#pragma once
#include <ecs/ecs_std.h>
#include <ecs/component_description.h>

namespace ecs
{
  template <typename T, typename U>
  struct ComponentInitializer
  {
    U value;
    ComponentInitializer(const U &value) : value(value) {}
    ComponentInitializer(U &&value) : value(std::move(value)) {}
  };

  struct ComponentPrefab : public ComponentDescription
  {
    ecs::any raw_component;
    void *raw_pointer;

    template <typename T, typename U>
    ComponentPrefab(const char *name, const ComponentInitializer<T, U> &prefab)
        : ComponentDescription(name, TypeIndex<T>::value), raw_component(prefab.value), raw_pointer(get<U>())
    {
    }

    template <typename T, typename U>
    ComponentPrefab(const char *name, ComponentInitializer<T, U> &&prefab)
        : ComponentDescription(name, TypeIndex<T>::value), raw_component(std::move(prefab.value)), raw_pointer(get<U>())
    {
    }

    template <typename T>
    ComponentPrefab(const char *name, const T &prefab)
        : ComponentDescription(name, TypeIndex<T>::value), raw_component(prefab), raw_pointer(get<T>())
    {
    }

    template <typename T>
    ComponentPrefab(const char *name, T &&prefab)
        : ComponentDescription(name, TypeIndex<T>::value), raw_component(std::move(prefab)), raw_pointer(get<T>())
    {
    }
    template <typename T>
    T *get()
    {
      return raw_component._Cast<T>();
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
  inline void sort_prefabs_by_names(ecs::vector<ComponentPrefab> &prefabs)
  {
    ecs::sort(prefabs.begin(), prefabs.end(), [](const auto &a, const auto &b)
              { return a.name < b.name; });
  }
}