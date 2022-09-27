#pragma once

#include <ecs/ecs_std.h>
#include <ecs/component_description.h>
#include <ecs/event.h>
#include <ecs/request.h>
#include <ecs/entity_id.h>

namespace ecs
{
  enum class AccessType
  {
    ReadOnly,
    ReadWrite
  };
  struct ArgumentDescription
  {
    ComponentDescription description;
    AccessType accessType;
    bool optional;

    template <typename T>
    ArgumentDescription(const char *name, AccessType access_type, bool optional)
        : description(name, TypeIndex<T>::value), accessType(access_type), optional(optional)
    {
    }
  };

  struct QueryCache
  {
    struct CachedArchetype
    {
      ecs::vector<int> componentIndexes;
      uint archetypeIndex;
    };
    ecs::vector<CachedArchetype> archetypes;
  };

  struct QueryDescription
  {
    const ecs::string file, name;
    const ecs::vector<ArgumentDescription> arguments;
    const ecs::vector<ComponentDescription> requiredComponents, requiredNotComponents;
    QueryCache *cache = nullptr;

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
    using SystemHandler = void (*)();
    const SystemHandler system;
    SystemDescription(const char *file,
                      const char *name,
                      QueryCache *cache,
                      ecs::vector<ArgumentDescription> &&arguments,
                      ecs::vector<ComponentDescription> &&required_components,
                      ecs::vector<ComponentDescription> &&required_not_components,
                      ecs::vector<ecs::string> &&before,
                      ecs::vector<ecs::string> &&after,
                      ecs::vector<ecs::string> &&tags,
                      SystemHandler system)
        : QueryDescription(file, name, cache, std::move(arguments), std::move(required_components), std::move(required_not_components)),
          OrderedDescription(std::move(before), std::move(after), std::move(tags)),
          system(system)
    {
    }
  };

  struct EventDescription : public QueryDescription, public OrderedDescription
  {
    using BroadcastEventHandler = void (*)(const Event &);
    using UnicastEventHandler = void (*)(EntityId, const Event &);
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
    using BroadcastRequestHandler = void (*)(Request &);
    using UnicastRequestHandler = void (*)(EntityId, Request &);
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
}
