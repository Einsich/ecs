#pragma once

#include <functional>
#include <ecs/ecs_std.h>
#include <ecs/component_description.h>
#include <ecs/event.h>
#include <ecs/request.h>
#include <ecs/entity_id.h>
#include <ecs/ska/flat_hash_map.hpp>
namespace ecs
{
  enum class AccessType
  {
    ReadOnly,
    ReadWrite,
    Copy
  };
  struct ArgumentDescription : public ComponentDescription
  {
    AccessType accessType;
    bool optional;
    bool isSingleton;

    ArgumentDescription(const char *name, int type_idx, AccessType access_type, bool optional, bool is_singleton)
        : ComponentDescription(name, type_idx), accessType(access_type), optional(optional), isSingleton(is_singleton)
    {
    }
  };

  struct QueryCache
  {
    ska::flat_hash_map<int, ecs::vector<int>> archetypes;
    bool noArchetype = false;
  };
  uint get_next_query_id();
  struct QueryDescription
  {
    uint uniqueId;
    ecs::string file, name;
    ecs::vector<ArgumentDescription> arguments;
    ecs::vector<ComponentDescription> requiredComponents, requiredNotComponents;
    QueryCache *cache = nullptr;
    bool noArchetype = false;

    void validate() const;

    QueryDescription(ecs::string &&file,
                     ecs::string &&name,
                     QueryCache *cache,
                     ecs::vector<ArgumentDescription> &&arguments,
                     ecs::vector<ComponentDescription> &&required_components,
                     ecs::vector<ComponentDescription> &&required_not_components)
        : uniqueId(get_next_query_id()), file(std::move(file)), name(std::move(name)),
          arguments(std::move(arguments)),
          requiredComponents(std::move(required_components)),
          requiredNotComponents(std::move(required_not_components)),
          cache(cache)
    {
      noArchetype = this->arguments.empty() && this->requiredComponents.empty() && this->requiredNotComponents.empty();
      cache->noArchetype = noArchetype;
      validate();
    }
  };

  struct OrderedDescription
  {
    ecs::vector<ecs::string> before, after, tags;
    OrderedDescription(ecs::vector<ecs::string> &&before, ecs::vector<ecs::string> &&after, ecs::vector<ecs::string> &&tags)
        : before(std::move(before)),
          after(std::move(after)),
          tags(std::move(tags))
    {
    }
  };

  struct SystemDescription : public QueryDescription, public OrderedDescription
  {
    ecs::string stage;
    using SystemHandler = std::function<void()>;
    SystemHandler system;
    SystemDescription(ecs::string &&file,
                      ecs::string &&name,
                      QueryCache *cache,
                      ecs::vector<ArgumentDescription> &&arguments,
                      ecs::vector<ComponentDescription> &&required_components,
                      ecs::vector<ComponentDescription> &&required_not_components,
                      ecs::string &&stage,
                      ecs::vector<ecs::string> &&before,
                      ecs::vector<ecs::string> &&after,
                      ecs::vector<ecs::string> &&tags,
                      SystemHandler system)
        : QueryDescription(std::move(file), std::move(name), cache, std::move(arguments), std::move(required_components), std::move(required_not_components)),
          OrderedDescription(std::move(before), std::move(after), std::move(tags)),
          stage(std::move(stage)), system(system)
    {
    }
  };

  struct EventDescription : public QueryDescription, public OrderedDescription
  {
    using BroadcastEventHandler = std::function<void(const Event &)>;
    using UnicastEventHandler = std::function<void(EntityId, const Event &)>;
    BroadcastEventHandler broadcastEventHandler;
    UnicastEventHandler unicastEventHandler;
    EventDescription(ecs::string &&file,
                     ecs::string &&name,
                     QueryCache *cache,
                     ecs::vector<ArgumentDescription> &&arguments,
                     ecs::vector<ComponentDescription> &&required_components,
                     ecs::vector<ComponentDescription> &&required_not_components,
                     ecs::vector<ecs::string> &&before,
                     ecs::vector<ecs::string> &&after,
                     ecs::vector<ecs::string> &&tags,
                     BroadcastEventHandler broadcast_event,
                     UnicastEventHandler unicast_event)
        : QueryDescription(std::move(file), std::move(name), cache, std::move(arguments), std::move(required_components), std::move(required_not_components)),
          OrderedDescription(std::move(before), std::move(after), std::move(tags)),
          broadcastEventHandler(broadcast_event),
          unicastEventHandler(unicast_event)
    {
    }
  };

  struct RequestDescription : public QueryDescription, public OrderedDescription
  {
    using BroadcastRequestHandler = std::function<void(Request &)>;
    using UnicastRequestHandler = std::function<void(EntityId, Request &)>;
    BroadcastRequestHandler broadcastRequestHandler;
    UnicastRequestHandler unicastRequestHandler;
    RequestDescription(ecs::string &&file,
                       ecs::string &&name,
                       QueryCache *cache,
                       ecs::vector<ArgumentDescription> &&arguments,
                       ecs::vector<ComponentDescription> &&required_components,
                       ecs::vector<ComponentDescription> &&required_not_components,
                       ecs::vector<ecs::string> &&before,
                       ecs::vector<ecs::string> &&after,
                       ecs::vector<ecs::string> &&tags,
                       BroadcastRequestHandler broadcast_request,
                       UnicastRequestHandler unicast_request)
        : QueryDescription(std::move(file), std::move(name), cache, std::move(arguments), std::move(required_components), std::move(required_not_components)),
          OrderedDescription(std::move(before), std::move(after), std::move(tags)),
          broadcastRequestHandler(broadcast_request),
          unicastRequestHandler(unicast_request)
    {
    }
  };
  struct QueryHandle
  {
    uint uniqueId = -1u;
  };
  struct SystemHandle
  {
    uint stageId = -1u;
    uint uniqueId = -1u;
  };
  struct EventHandle
  {
    uint typeId = -1u;
    uint uniqueId = -1u;
  };
  struct RequestHandle
  {
    uint typeId = -1u;
    uint uniqueId = -1u;
  };


  QueryHandle register_query(ecs::string &&file,
                             ecs::string &&name,
                             QueryCache *cache,
                             ecs::vector<ArgumentDescription> &&arguments,
                             ecs::vector<ComponentDescription> &&required_components,
                             ecs::vector<ComponentDescription> &&required_not_components);
  SystemHandle register_system(ecs::string &&file,
                               ecs::string &&name,
                               QueryCache *cache,
                               ecs::vector<ArgumentDescription> &&arguments,
                               ecs::vector<ComponentDescription> &&required_components,
                               ecs::vector<ComponentDescription> &&required_not_components,
                               ecs::string &&stage,
                               ecs::vector<ecs::string> &&before,
                               ecs::vector<ecs::string> &&after,
                               ecs::vector<ecs::string> &&tags,
                               SystemDescription::SystemHandler system);
  EventHandle register_event(ecs::string &&file,
                             ecs::string &&name,
                             QueryCache *cache,
                             ecs::vector<ArgumentDescription> &&arguments,
                             ecs::vector<ComponentDescription> &&required_components,
                             ecs::vector<ComponentDescription> &&required_not_components,
                             ecs::vector<ecs::string> &&before,
                             ecs::vector<ecs::string> &&after,
                             ecs::vector<ecs::string> &&tags,
                             EventDescription::BroadcastEventHandler broadcast_event,
                             EventDescription::UnicastEventHandler unicast_event,
                             event_t event_id);
  RequestHandle register_request(ecs::string &&file,
                                 ecs::string &&name,
                                 QueryCache *cache,
                                 ecs::vector<ArgumentDescription> &&arguments,
                                 ecs::vector<ComponentDescription> &&required_components,
                                 ecs::vector<ComponentDescription> &&required_not_components,
                                 ecs::vector<ecs::string> &&before,
                                 ecs::vector<ecs::string> &&after,
                                 ecs::vector<ecs::string> &&tags,
                                 RequestDescription::BroadcastRequestHandler broadcast_request,
                                 RequestDescription::UnicastRequestHandler unicast_request,
                                 request_t request_id);

  bool remove_query(QueryHandle handle, bool free_cache);
  bool remove_system(SystemHandle handle, bool free_cache);
  bool remove_event(EventHandle handle, bool free_cache);
  bool remove_request(RequestHandle handle, bool free_cache);

  struct FileRegistrationHelper
  {
    FileRegistrationHelper(void (*pull_function)())
    {

      extern void file_registration(void (*)());
      file_registration(pull_function);
    }
  };
#define ECS_FILE_REGISTRATION(pull_function) \
  static ecs::FileRegistrationHelper __CONCAT__(fileRegistrator, __COUNTER__)(pull_function);
}
