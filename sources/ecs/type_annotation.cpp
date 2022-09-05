#include <ecs/type_annotation.h>
#include <ecs/ska/flat_hash_map.hpp>
namespace ecs
{
  using NameTable = ska::flat_hash_map<ecs::string, int, ska::power_of_two_std_hash<ecs::string>>;
  static NameTable &type_name_to_index_table()
  {
    static NameTable typeNameToIndex;
    return typeNameToIndex;
  }
  static ecs::vector<TypeAnnotation> allRegisteredTypes;

  void register_type(int type_index, TypeAnnotation annotation)
  {
    type_name_to_index_table().emplace(annotation.name, type_index);
    ECS_ASSERT((int)allRegisteredTypes.size() == type_index);
    allRegisteredTypes.push_back(annotation);
  }

  int type_name_to_index(const char *name)
  {
    auto it = type_name_to_index_table().find(name);
    return it != type_name_to_index_table().end() ? it->second : -1;
  }

  const ecs::vector<TypeAnnotation> &get_all_registered_types()
  {
    return allRegisteredTypes;
  }

  int get_next_type_index()
  {
    static int typeIndexCounter = 0;
    return typeIndexCounter++;
  }
}