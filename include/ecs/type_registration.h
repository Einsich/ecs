#pragma once
#include <ecs/type_annotation.h>
#include <ecs/component_prefab.h>

namespace ecs
{
  template<typename T>
  struct DefaultTypeFabric : public TypeFabric
  {
    DefaultTypeFabric(ecs::Type type = Type::DefaultType, bool has_prefab_ctor = false, bool has_awaiter = false):
      TypeFabric(type, has_prefab_ctor, has_awaiter)
    {}

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


  template <typename T>
  const TypeAnnotation *get_type_annotation()
  {
    const auto &types = get_all_registered_types();
    return TypeIndex<T>::value < (int)types.size() ? &types[TypeIndex<T>::value] : nullptr;
  }

  template <typename T>
  void type_registration(const char *type_name, const TypeFabric *type_fabric)
  {
    ECS_ASSERT(type_fabric != nullptr);
    extern int get_next_type_index();
    extern void register_type(int, TypeAnnotation);

    ECS_ASSERT(TypeIndex<T>::value == -1);
    TypeIndex<T>::value = get_next_type_index();
    register_type(
        TypeIndex<T>::value,
        TypeAnnotation{
            type_name,
            sizeof(T),
            type_fabric});
  }
  template <typename T, Type type = Type::DefaultType>
  struct RegistrationHelper
  {
    DefaultTypeFabric<T> fabric;
    RegistrationHelper(const char *name): fabric(type)
    {
      type_registration<T>(name, &fabric);
    }
  };
  template <typename T>
  struct RegistrationHelperFabric
  {
    RegistrationHelperFabric(const char *name, const TypeFabric *explicit_type_fabric)
    {
      type_registration<T>(name, explicit_type_fabric);
    }
  };

#define ECS_TYPE_REGISTRATION(TYPE,              \
                              NAME,              \
                              TYPE_TIP)          \
  static ecs::RegistrationHelper<TYPE, TYPE_TIP> \
      __CONCAT__(registrator, __COUNTER__)(NAME);

#define ECS_TYPE_REGISTRATION_WITH_FABRIC(TYPE,        \
                              NAME,                    \
                              EXPLICIT_FABRIC)         \
  static ecs::RegistrationHelperFabric<TYPE>           \
      __CONCAT__(registrator, __COUNTER__)(NAME, EXPLICIT_FABRIC);
}