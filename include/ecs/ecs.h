#pragma once
#include <ecs/type_annotation.h>
#include <ecs/query_description.h>
#include <ecs/chunk_policy.h>
#include <ecs/entity_prefab.h>
#include <ecs/query_manager.h>
#include <ecs/base_events.h>
#include <ecs/system_stage.h>
#include <ecs/singleton.h>

namespace ecs
{
  void init(bool register_base_types = true);
  void pull_registered_files();
  void init_stages(const ecs::vector<SystemStage> &stages);
  void update_query_manager();
  void perform_systems();
  uint add_archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power);
  uint add_archetype(const ecs::vector<ComponentPrefab> &descriptions, SizePolicy chunk_power);
  EntityId create_entity_immediate(prefab_id id, ecs::vector<ComponentPrefab> &&overrides_list = {});
  EntityId create_entity(prefab_id id, ecs::vector<ComponentPrefab> &&overrides_list = {});
  EntityId create_entity(const char *prefab_name, ecs::vector<ComponentPrefab> &&overrides_list = {});

  void destroy_entity(EntityId eid);
  void destroy_all_entities();
  void update_archetype_manager();

  template <typename E>
  std::enable_if_t<std::is_base_of_v<ecs::Event, E>, void> send_event_immediate(const E &event)
  {
    // TODO add explanation
    ECS_ASSERT_RETURN(EventIndex<E>::value != -1, );
    get_query_manager().send_event_immediate(event, EventIndex<E>::value);
  }

  template <typename E>
  std::enable_if_t<std::is_base_of_v<ecs::Event, E>, void> send_event_immediate(ecs::EntityId eid, const E &event)
  {
    // TODO add explanation
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

  template <typename R>
  std::enable_if_t<std::is_base_of_v<ecs::Request, R>, void> send_request(R &request)
  {
    ECS_ASSERT_RETURN(RequestIndex<R>::value != -1, );
    get_query_manager().send_request(request, RequestIndex<R>::value);
  }

  template <typename R>
  std::enable_if_t<std::is_base_of_v<ecs::Request, R>, void> send_request(ecs::EntityId eid, R &request)
  {
    ECS_ASSERT_RETURN(RequestIndex<R>::value != -1, );
    get_query_manager().send_request(eid, request, RequestIndex<R>::value);
  }

  // take into account is type singleton
  template <typename T>
  int get_type_index()
  {
    if constexpr (ecs::is_singleton<T>())
    {
      return ecs::SingletonIndex<T>::value;
    }
    else
    {
      return ecs::TypeIndex<T>::value;
    }
  }

  void set_system_tags(const ecs::vector<ecs::string> &tags);
  void set_system_tags(ecs::vector<ecs::string> &&tags);

  struct Tag {};
}

#define SYSTEM(...) static void
#define QUERY(...)
#define EVENT(...) static void
#define REQUEST(...) static void