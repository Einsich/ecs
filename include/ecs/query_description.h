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
    ecs::string file, name;
    ecs::vector<ArgumentDescription> arguments;
    ecs::vector<ComponentDescription> requiredComponents, requiredNotComponents;

    QueryDescription(const char *file,
                     const char *name,
                     ecs::vector<ArgumentDescription> &&arguments,
                     ecs::vector<ComponentDescription> &&required_components,
                     ecs::vector<ComponentDescription> &&required_not_components)
        : file(file), name(name),
          arguments(std::move(arguments)),
          requiredComponents(std::move(required_components)),
          requiredNotComponents(std::move(required_not_components))
    {
    }
  };

  struct SystemDescription : public QueryDescription
  {
    using SystemHandler = void (*)();
    SystemHandler system;
    SystemDescription(const char *file,
                      const char *name,
                      ecs::vector<ArgumentDescription> &&arguments,
                      ecs::vector<ComponentDescription> &&required_components,
                      ecs::vector<ComponentDescription> &&required_not_components,
                      SystemHandler system)
        : QueryDescription(file, name, std::move(arguments), std::move(required_components), std::move(required_not_components)),
          system(system)
    {
    }
  };

  struct EventDescription : public QueryDescription
  {
    using BroadcastEventHandler = void (*)(const Event &);
    using UnicastEventHandler = void (*)(EntityId, const Event &);
    BroadcastEventHandler broadcastEventHandler;
    UnicastEventHandler unicastEventHandler;
    EventDescription(const char *file,
                     const char *name,
                     ecs::vector<ArgumentDescription> &&arguments,
                     ecs::vector<ComponentDescription> &&required_components,
                     ecs::vector<ComponentDescription> &&required_not_components,
                     BroadcastEventHandler broadcast_event,
                     UnicastEventHandler unicast_event)
        : QueryDescription(file, name, std::move(arguments), std::move(required_components), std::move(required_not_components)),
          broadcastEventHandler(broadcast_event),
          unicastEventHandler(unicast_event)
    {
    }
  };

  struct RequestDescription : public QueryDescription
  {
    using BroadcastRequestHandler = void (*)(const Request &);
    using UnicastRequestHandler = void (*)(EntityId, const Request &);
    BroadcastRequestHandler broadcastRequestHandler;
    UnicastRequestHandler unicastRequestHandler;
    RequestDescription(const char *file,
                       const char *name,
                       ecs::vector<ArgumentDescription> &&arguments,
                       ecs::vector<ComponentDescription> &&required_components,
                       ecs::vector<ComponentDescription> &&required_not_components,
                       BroadcastRequestHandler broadcast_request,
                       UnicastRequestHandler unicast_request)
        : QueryDescription(file, name, std::move(arguments), std::move(required_components), std::move(required_not_components)),
          broadcastRequestHandler(broadcast_request),
          unicastRequestHandler(unicast_request)
    {
    }
  };
}
