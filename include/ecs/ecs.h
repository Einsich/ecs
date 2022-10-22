#pragma once
#include <ecs/type_annotation.h>
#include <ecs/query_description.h>
#include <ecs/chunk_policy.h>
#include <ecs/entity_prefab.h>
#include <ecs/query_manager.h>

namespace ecs
{
  void update_query_manager();
  void perform_systems();
  uint add_archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power);
  uint add_archetype(const ecs::vector<ComponentPrefab> &descriptions, SizePolicy chunk_power);
  void create_entity_immediate(const EntityPrefab &prefabs_list, EntityPrefab &&overrides_list = {}, SizePolicy chunk_power = SizePolicy::Hundreds);

  template <typename E>
  std::enable_if_t<std::is_base_of_v<ecs::Event, E>, void> send_event_immediate(const E &event)
  {
    ECS_ASSERT_RETURN(EventIndex<E>::value != -1, );
    get_query_manager().send_event_immediate(event, EventIndex<E>::value);
  }

  template <typename E>
  std::enable_if_t<std::is_base_of_v<ecs::Event, E>, void> send_event_immediate(ecs::EntityId eid, const E &event)
  {
    ECS_ASSERT_RETURN(EventIndex<E>::value != -1, );
    get_query_manager().send_event_immediate(eid, event, EventIndex<E>::value);
  }

  template <typename E>
  std::enable_if_t<std::is_base_of_v<ecs::Event, E>, void> send_event(const E &event)
  {
    ECS_ASSERT_RETURN(EventIndex<E>::value != -1, );
    get_query_manager().send_event_deffered(event, EventIndex<E>::value);
  }

  template <typename E>
  std::enable_if_t<std::is_base_of_v<ecs::Event, E>, void> send_event(ecs::EntityId eid, const E &event)
  {
    ECS_ASSERT_RETURN(EventIndex<E>::value != -1, );
    get_query_manager().send_event_deffered(eid, event, EventIndex<E>::value);
  }
}