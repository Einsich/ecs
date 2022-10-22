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
    // TODO: rework me to optimize allocations
    ecs::queue<std::function<void()>> eventsQueue;

    bool queryInvalidated = false;
    bool systemsInvalidated = false;
    bool eventsInvalidated = false;
    bool requestsInvalidated = false;

    void clearCache();
    void rebuildDependencyGraph();
    // should check all changes and update invalidated structures
    void update();

    void addArchetypeToCache(uint archetype_idx);

    void send_event_immediate(const ecs::Event &event, event_t event_id) const;
    void send_event_immediate(EntityId eid, const ecs::Event &event, event_t event_id) const;

    template <typename T>
    void send_event_deffered(const T &event, event_t event_id)
    {
      eventsQueue.push([event, event_id, this]()
                       { send_event_immediate(event, event_id); });
    }
    template <typename T>
    void send_event_deffered(EntityId eid, const T &event, event_t event_id)
    {
      eventsQueue.push([eid, event, event_id, this]()
                       { send_event_immediate(eid, event, event_id); });
    }
  };
  QueryManager &get_query_manager();

  struct DefferedEvent
  {
    event_t event_id;
    uint idx;
  };

}
