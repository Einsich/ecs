#include <ecs/type_annotation.h>


namespace ecs
{
  TypeFabric::TypeFabric(const char *name, Type type, bool has_prefab_ctor, bool has_awaiter, size_t size):
    name(name),
    trivialCopy(type&TrivialCopyConstructor),
    trivialMove(type&TrivialMoveConstructor),
    trivialDestruction(type&TrivialDestructor),
    hasPrefabCtor(has_prefab_ctor),
    hasAwaiter(has_awaiter),
    sizeOf(size)
  {
    if (linked_list_head != nullptr)
      linked_list_head->nextFabric = this;
    linked_list_head = this;
  }

}