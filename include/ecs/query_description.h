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

  struct QueryDescription
  {
    const ecs::string file, name;
    const ecs::vector<ArgumentDescription> arguments;
    const ecs::vector<ComponentDescription> requiredComponents, requiredNotComponents;
    QueryCache *cache = nullptr;
    bool noArchetype = false;

    void validate() const;

    QueryDescription(const char *file,
                     const char *name,
                     QueryCache *cache,
                     ecs::vector<ArgumentDescription> &&arguments,
                     ecs::vector<ComponentDescription> &&required_components,
                     ecs::vector<ComponentDescription> &&required_not_components)
        : file(file), name(name),
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
    const ecs::vector<ecs::string> before, after, tags;
    OrderedDescription(ecs::vector<ecs::string> &&before, ecs::vector<ecs::string> &&after, ecs::vector<ecs::string> &&tags)
        : before(std::move(before)),
          after(std::move(after)),
          tags(std::move(tags))
    {
    }
  };

  struct SystemDescription : public QueryDescription, public OrderedDescription
  {
    const ecs::string stage;
    using SystemHandler = std::function<void()>;
    const SystemHandler system;
    SystemDescription(const char *file,
                      const char *name,
                      QueryCache *cache,
                      ecs::vector<ArgumentDescription> &&arguments,
                      ecs::vector<ComponentDescription> &&required_components,
                      ecs::vector<ComponentDescription> &&required_not_components,
                      ecs::string &&stage,
                      ecs::vector<ecs::string> &&before,
                      ecs::vector<ecs::string> &&after,
                      ecs::vector<ecs::string> &&tags,
                      SystemHandler system)
        : QueryDescription(file, name, cache, std::move(arguments), std::move(required_components), std::move(required_not_components)),
          OrderedDescription(std::move(before), std::move(after), std::move(tags)),
          stage(std::move(stage)), system(system)
    {
    }
  };

  struct EventDescription : public QueryDescription, public OrderedDescription
  {
    using BroadcastEventHandler = std::function<void(const Event &)>;
    using UnicastEventHandler = std::function<void(EntityId, const Event &)>;
    const BroadcastEventHandler broadcastEventHandler;
    const UnicastEventHandler unicastEventHandler;
    EventDescription(const char *file,
                     const char *name,
                     QueryCache *cache,
                     ecs::vector<ArgumentDescription> &&arguments,
                     ecs::vector<ComponentDescription> &&required_components,
                     ecs::vector<ComponentDescription> &&required_not_components,
                     ecs::vector<ecs::string> &&before,
                     ecs::vector<ecs::string> &&after,
                     ecs::vector<ecs::string> &&tags,
                     BroadcastEventHandler broadcast_event,
                     UnicastEventHandler unicast_event)
        : QueryDescription(file, name, cache, std::move(arguments), std::move(required_components), std::move(required_not_components)),
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
    const BroadcastRequestHandler broadcastRequestHandler;
    const UnicastRequestHandler unicastRequestHandler;
    RequestDescription(const char *file,
                       const char *name,
                       QueryCache *cache,
                       ecs::vector<ArgumentDescription> &&arguments,
                       ecs::vector<ComponentDescription> &&required_components,
                       ecs::vector<ComponentDescription> &&required_not_components,
                       ecs::vector<ecs::string> &&before,
                       ecs::vector<ecs::string> &&after,
                       ecs::vector<ecs::string> &&tags,
                       BroadcastRequestHandler broadcast_request,
                       UnicastRequestHandler unicast_request)
        : QueryDescription(file, name, cache, std::move(arguments), std::move(required_components), std::move(required_not_components)),
          OrderedDescription(std::move(before), std::move(after), std::move(tags)),
          broadcastRequestHandler(broadcast_request),
          unicastRequestHandler(unicast_request)
    {
    }
  };

  void register_query(QueryDescription &&query);
  void register_system(SystemDescription &&system);
  void register_event(EventDescription &&event, event_t event_id);
  void register_request(RequestDescription &&request, request_t event_id);

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
