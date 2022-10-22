#pragma once

#include <ecs/ecs_std.h>
#include <ecs/ska/flat_hash_map.hpp>

namespace ecs
{
  template <typename Description>
  struct DescriptionStorage
  {
    using NameTable = ska::flat_hash_map<ecs::string, int, ska::power_of_two_std_hash<ecs::string>>;

    NameTable typeNameToIndex;

    ecs::vector<Description> allRegisteredTypes;

    void register_type(int type_index, Description &&annotation)
    {
      typeNameToIndex.emplace(annotation.name, type_index);
      ECS_ASSERT((int)allRegisteredTypes.size() == type_index);
      allRegisteredTypes.push_back(std::move(annotation));
    }

    int type_name_to_index(const char *name) const
    {
      auto it = typeNameToIndex.find(name);
      return it != typeNameToIndex.end() ? it->second : -1;
    }

    const ecs::vector<Description> &get_all_registered_types() const
    {
      return allRegisteredTypes;
    }

    int get_next_type_index() const
    {
      static int typeIndexCounter = 0;
      return typeIndexCounter++;
    }
  };
}

#define USE_DESCRIPTION_STORAGE(T,                                    \
                                register_type_func,                   \
                                type_name_to_index_func,              \
                                get_all_registered_types_func,        \
                                get_next_type_index_func)             \
  namespace ecs                                                       \
  {                                                                   \
    static DescriptionStorage<T> &get_storage()                       \
    {                                                                 \
      static DescriptionStorage<T> storage;                           \
      return storage;                                                 \
    }                                                                 \
    void register_type_func(int type_index, T annotation)             \
    {                                                                 \
      get_storage().register_type(type_index, std::move(annotation)); \
    }                                                                 \
    int type_name_to_index_func(const char *name)                     \
    {                                                                 \
      return get_storage().type_name_to_index(name);                  \
    }                                                                 \
    const ecs::vector<T> &get_all_registered_types_func()             \
    {                                                                 \
      return get_storage().get_all_registered_types();                \
    }                                                                 \
    int get_next_type_index_func()                                    \
    {                                                                 \
      return get_storage().get_next_type_index();                     \
    }                                                                 \
  }