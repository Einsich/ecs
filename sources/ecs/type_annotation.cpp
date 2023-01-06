#include <ecs/type_annotation.h>
#include <ecs/type_description_storage.h>

USE_DESCRIPTION_STORAGE(ecs::TypeAnnotation, update_type, register_type, type_name_to_index, get_all_registered_types, get_next_type_index)

namespace ecs
{

  const char *type_name(uint type_idx)
  {
    const auto &types = ecs::get_all_registered_types();
    return type_idx < types.size() ? types[type_idx].name.c_str() : nullptr;
  }

}