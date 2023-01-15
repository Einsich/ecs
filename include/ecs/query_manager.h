#pragma once
#include <ecs/query_description.h>

namespace ecs
{
  using stage_id = uint;
  struct QueryManager
  {

    ecs::vector<ecs::string> tags;

    ecs::vector<ecs::unique_ptr<QueryDescription>> queries;
    ska::flat_hash_map<ecs::string, stage_id> stagesMap;
    ecs::vector<ecs::vector<ecs::unique_ptr<SystemDescription>>> systems;
    ecs::vector<ecs::vector<ecs::unique_ptr<EventDescription>>> events;
    ecs::vector<ecs::vector<ecs::unique_ptr<RequestDescription>>> requests;

    ecs::vector<ecs::vector<SystemDescription *>> activeSystems;
    ecs::vector<ecs::vector<EventDescription *>> activeEvents;
    ecs::vector<ecs::vector<RequestDescription *>> activeRequests;
    // TODO: rework me to optimize allocations
    ecs::queue<std::function<void()>> eventsQueue;

    bool queryInvalidated = false;
    bool systemsInvalidated = false;
    bool eventsInvalidated = false;
    bool requestsInvalidated = false;

    bool requireUpdate() const;
    void clearCache();
    void rebuildDependencyGraph();
    // should check all changes and update invalidated structures
    void performDefferedEvents();
    void invalidate();

    void addArchetypeToCache(uint archetype_idx);

    void sendEventImmediate(const ecs::Event &event, event_t event_id) const;
    void sendEventImmediate(EntityId eid, const ecs::Event &event, event_t event_id) const;

    template <typename T>
    void sendEventDeffered(const T &event, event_t event_id)
    {
      eventsQueue.push([event, event_id, this]()
                       { sendEventImmediate(event, event_id); });
    }
    template <typename T>
    void sendEventDeffered(EntityId eid, const T &event, event_t event_id)
    {
      eventsQueue.push([eid, event, event_id, this]()
                       { sendEventImmediate(eid, event, event_id); });
    }
    void sendRequest(ecs::Request &request, request_t request_id) const;
    void sendRequest(EntityId eid, ecs::Request &request, request_t request_id) const;


    stage_id findStageId(const char *stage_name) const;
    void performStage(const char *stage) const;
    void performStage(stage_id stage) const;

  };
  QueryManager &get_query_manager();

}
