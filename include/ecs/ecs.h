#pragma once
#include <ecs/type_annotation.h>
#include <ecs/query_description.h>
#include <ecs/chunk_policy.h>
#include <ecs/entity_prefab.h>
#include <ecs/query_manager.h>
#include <ecs/base_events.h>
#include <ecs/singleton.h>

namespace ecs
{
  void init(bool register_base_types = true);
  void pull_registered_files();
  void perform_deffered_events();
  stage_id find_stage(const char *stage);
  void perform_stage(const char *stage);
  void perform_stage(stage_id stage);
  uint add_archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power);
  uint add_archetype(const ecs::vector<ComponentPrefab> &descriptions, SizePolicy chunk_power);
  EntityId create_entity_immediate(prefab_id id, ecs::vector<ComponentPrefab> &&overrides_list = {});
  EntityId create_entity_immediate(const char *prefab_name, ecs::vector<ComponentPrefab> &&overrides_list = {});
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
    get_query_manager().sendEventImmediate(event, EventIndex<E>::value);
  }

  template <typename E>
  std::enable_if_t<std::is_base_of_v<ecs::Event, E>, void> send_event_immediate(ecs::EntityId eid, const E &event)
  {
    // TODO add explanation
    ECS_ASSERT_RETURN(EventIndex<E>::value != -1, );
    get_query_manager().sendEventImmediate(eid, event, EventIndex<E>::value);
  }

  template <typename E>
  std::enable_if_t<std::is_base_of_v<ecs::Event, E>, void> send_event(const E &event)
  {
    ECS_ASSERT_RETURN(EventIndex<E>::value != -1, );
    get_query_manager().sendEventDeffered(event, EventIndex<E>::value);
  }

  template <typename E>
  std::enable_if_t<std::is_base_of_v<ecs::Event, E>, void> send_event(ecs::EntityId eid, const E &event)
  {
    ECS_ASSERT_RETURN(EventIndex<E>::value != -1, );
    get_query_manager().sendEventDeffered(eid, event, EventIndex<E>::value);
  }

  template <typename R>
  std::enable_if_t<std::is_base_of_v<ecs::Request, R>, void> send_request(R &request)
  {
    ECS_ASSERT_RETURN(RequestIndex<R>::value != -1, );
    get_query_manager().sendRequest(request, RequestIndex<R>::value);
  }

  template <typename R>
  std::enable_if_t<std::is_base_of_v<ecs::Request, R>, void> send_request(ecs::EntityId eid, R &request)
  {
    ECS_ASSERT_RETURN(RequestIndex<R>::value != -1, );
    get_query_manager().sendRequest(eid, request, RequestIndex<R>::value);
  }

  // take into account is type singleton
  template <typename T>
  const TypeFabric * get_type_index()
  {
    if constexpr (ecs::is_singleton<T>())
    {
      return nullptr;
    }
    else
    {
      return ecs::TypeIndex<T>::value;
    }
  }

  void set_system_tags(const ecs::vector<ecs::string> &tags);
  void set_system_tags(ecs::vector<ecs::string> &&tags);

  struct Tag {};

  using ProfilerPush = void (*)(const char* label);
  using ProfilerPop = void (*)();
  //you can override this for custom profiling
  extern ProfilerPush ecs_profiler_push;
  extern ProfilerPop ecs_profiler_pop;
  extern bool ecs_profiler_enabled;
}

#define SYSTEM(...) static void
#define QUERY(...)
#define EVENT(...) static void
#define REQUEST(...) static void

#define ECS_PULL_DECLARATION(PULL_VAR) extern size_t PULL_VAR;
#define ECS_PULL_DEFINITION(PULL_VAR) size_t PULL_VAR = (uintptr_t)&PULL_VAR;