#pragma once
#include <ecs/type_annotation.h>
#include <ecs/component_prefab.h>

namespace ecs
{
  template<typename T>
  struct DefaultTypeFabric : public TypeFabric
  {
    DefaultTypeFabric(const char *name, ecs::Type type = Type::DefaultType, bool has_prefab_ctor = false, bool has_awaiter = false):
      TypeFabric(name, type, has_prefab_ctor, has_awaiter, sizeof(T))
    {
      ECS_ASSERT(TypeIndex<T>::value == nullptr);
      TypeIndex<T>::value = this;
    }

    virtual void copy_constructor(void *raw_memory, const void *source) const override
    {
      new (raw_memory) T(*(const T *)source);
    }
    virtual void move_constructor(void *raw_memory, void *source) const override
    {
      new (raw_memory) T(std::move(*(T *)source));
    }
    virtual void destructor(void *memory) const override
    {
      ((T *)memory)->~T();
    }
    virtual void prefab_constructor(void *, const ecs::ComponentPrefab &, bool ) const override
    { ECS_ASSERT(false); }
    virtual bool component_awaiter(const ecs::ComponentPrefab &) const override
    { ECS_ASSERT(false); return true; }
    virtual void await_contructor(void *, const ecs::ComponentPrefab &, bool ) const override
    { ECS_ASSERT(false); }
  };



#define ECS_TYPE_REGISTRATION(TYPE,              \
                              NAME,              \
                              TYPE_TIP)          \
  static ecs::DefaultTypeFabric<TYPE> \
      __CONCAT__(registrator, __COUNTER__)(NAME, TYPE_TIP);

}