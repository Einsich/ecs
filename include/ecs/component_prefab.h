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
    template <typename T>
    static const void *get_raw_mem(const ecs::any &raw_component)
    {
      return std::any_cast<T*>(raw_component);
    }
    ecs::any raw_component;
    const void *(*raw_pointer_getter)(const ecs::any &);

    void *get_raw_memory()
    {
      return const_cast<void *>(static_cast<const ComponentPrefab *>(this)->get_raw_memory());
    }
    const void *get_raw_memory() const
    {
      return raw_pointer_getter(raw_component);
    }

    template <typename T, typename U>
    ComponentPrefab(const char *name, const ComponentInitializer<T, U> &prefab)
        : ComponentDescription(name, TypeIndex<T>::value), raw_component(prefab.value), raw_pointer_getter(get_raw_mem<U>)
    {
    }

    template <typename T, typename U>
    ComponentPrefab(const char *name, ComponentInitializer<T, U> &&prefab)
        : ComponentDescription(name, TypeIndex<T>::value), raw_component(std::move(prefab.value)), raw_pointer_getter(get_raw_mem<U>)
    {
    }

    template <typename U>
    ComponentPrefab(const char *name, int type_index, const U &prefab)
        : ComponentDescription(name, type_index), raw_component(prefab), raw_pointer_getter(get_raw_mem<U>)
    {
    }

    template <typename U>
    ComponentPrefab(const char *name, int type_index, const U *prefab)
        : ComponentDescription(name, type_index), raw_component(prefab), raw_pointer_getter(get_raw_mem<U*>)
    {
    }
    template <typename U>
    ComponentPrefab(const char *name, int type_index, U &&prefab)
        : ComponentDescription(name, type_index), raw_component(prefab), raw_pointer_getter(get_raw_mem<U>)
    {
    }
    template <typename T>
    ComponentPrefab(const char *name, const T &prefab)
        : ComponentDescription(name, TypeIndex<T>::value), raw_component(prefab), raw_pointer_getter(get_raw_mem<T>)
    {
    }
    template <typename T>
    ComponentPrefab(const char *name, T &prefab)
        : ComponentDescription(name, TypeIndex<T>::value), raw_component(prefab), raw_pointer_getter(get_raw_mem<T>)
    {
    }

    template <typename T>
    ComponentPrefab(const char *name, T &&prefab)
        : ComponentDescription(name, TypeIndex<T>::value), raw_component(std::move(prefab)), raw_pointer_getter(get_raw_mem<T>)
    {
    }
    
    template <typename T>
    bool is() const
    {
      return raw_component.type() == typeid(T);
    }
    template <typename T>
    T &get()
    {
      return std::any_cast<T&>(raw_component);
    }
    template <typename T>
    const T &get() const
    {
      return std::any_cast<const T&>(raw_component);
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