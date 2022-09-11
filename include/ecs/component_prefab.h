#pragma once
#include <ecs/ecs_std.h>
#include <ecs/hash.h>

namespace ecs
{
  struct ComponentDescription
  {
    ecs::string name;
    uint nameHash;
    uint typeIndex;
    ComponentDescription() = default;

    ComponentDescription(const char *name, int type_index)
        : name(name), nameHash(ecs::hash(name)), typeIndex(type_index)
    {
      ECS_ASSERT(type_index >= 0);
    }
  };
  template <typename T, typename U>
  struct ComponentInitializer
  {
    U value;
    ComponentInitializer(const U &value) : value(value) {}
    ComponentInitializer(U &&value) : value(std::move(value)) {}
  };

  struct ComponentPrefab
  {
    ComponentDescription description;
    ecs::any raw_component;
    void *raw_pointer;

    template <typename T, typename U>
    ComponentPrefab(const char *name, const ComponentInitializer<T, U> &prefab)
        : description(name, TypeIndex<T>::value), raw_component(prefab.value), raw_pointer(get<U>())
    {
    }

    template <typename T, typename U>
    ComponentPrefab(const char *name, ComponentInitializer<T, U> &&prefab)
        : description(name, TypeIndex<T>::value), raw_component(std::move(prefab.value)), raw_pointer(get<U>())
    {
    }

    template <typename T>
    ComponentPrefab(const char *name, const T &prefab)
        : description(name, TypeIndex<T>::value), raw_component(prefab), raw_pointer(get<T>())
    {
    }

    template <typename T>
    ComponentPrefab(const char *name, T &&prefab)
        : description(name, TypeIndex<T>::value), raw_component(std::move(prefab)), raw_pointer(get<T>())
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
  void sort_prefabs_by_names(ecs::vector<ComponentPrefab> &prefabs)
  {
    ecs::sort(prefabs.begin(), prefabs.end(), [](const auto &a, const auto &b)
              { return a.description.name < b.description.name; });
  }
}