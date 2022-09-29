#pragma once
#include <ecs/query_description.h>

namespace ecs
{
  struct QueryManager
  {

    ecs::vector<ecs::string> tags;

    ecs::vector<QueryDescription> queries;
    ecs::vector<SystemDescription> systems;
    ecs::vector<ecs::vector<EventDescription>> events;
    ecs::vector<ecs::vector<RequestDescription>> requests;

    ecs::vector<SystemDescription *> activeSystems;
    ecs::vector<ecs::vector<EventDescription *>> activeEvents;
    ecs::vector<ecs::vector<RequestDescription *>> activeRequests;

    bool queryInvalidated = false;
    bool systemsInvalidated = false;
    bool eventsInvalidated = false;
    bool requestsInvalidated = false;

    void clearCache();
    void rebuildDependencyGraph();
    // should check all changes and update invalidated structures
    void update();

    void addArchetypeToCache(uint archetype_idx);
  };
  QueryManager &get_query_manager();
}
