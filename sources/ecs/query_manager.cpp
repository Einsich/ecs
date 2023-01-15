#include <ecs/query_manager.h>
#include <ecs/ska/flat_hash_map.hpp>
#include <ecs/archetype_manager.h>
#include "profiler_scope.h"

namespace ecs
{
  enum NodeState
  {
    Black,
    Gray,
    White,
    Cycle
  };

  std::function<void(ecs::vector<uint>)> loger;

  using Edges = ecs::vector<ecs::vector<uint>>;

  static bool log_cycle(uint c, uint v, const Edges &edges, const ecs::vector<NodeState> &used, ecs::vector<uint> &cycle)
  {
    if (c == v)
      return true;
    for (uint to : edges[v])
    {
      if (used[to] == Gray)
      {
        if (log_cycle(c, to, edges, used, cycle))
        {
          cycle.push_back(to);
          return true;
        }
      }
    }
    return false;
  }

  static void dfs(uint v, const Edges &edges, ecs::vector<NodeState> &used, ecs::vector<uint> &answer)
  {
    used[v] = Gray;
    for (uint to : edges[v])
    {
      if (used[to] == Gray)
      {
        ECS_ERROR("cycle detected :");
        ecs::vector<uint> cycle;
        log_cycle(v, to, edges, used, cycle);
        if (loger)
          loger(cycle);
        used[to] = Cycle;
        continue;
      }
      if (used[to] == Black)
        dfs(to, edges, used, answer);
    }
    answer.push_back(v);
    used[v] = White;
  }

  template <typename Description>
  static void topological_sort(ecs::vector<Description *> &queries)
  {
    loger = [&](const auto &v)
    {
      for (uint x : v)
        ECS_ERROR("%s -> ", queries[x]->name.c_str());
      ECS_ERROR("\n");
    };
    auto size = queries.size();
    Edges edge(size);
    ecs::vector<NodeState> used(size, Black);
    ecs::vector<uint> answer;
    answer.reserve(size);
    ska::flat_hash_map<string, uint, ska::power_of_two_std_hash<ecs::string>> nameMap;
    for (uint i = 0; i < size; i++)
      nameMap[queries[i]->name] = i;
    for (uint i = 0; i < size; i++)
    {
      for (const string &before : queries[i]->before)
      {
        auto it = nameMap.find(before);
        if (it != nameMap.end())
        {
          if (i == it->second)
          {
            ECS_ERROR("system %s has itself in before", queries[i]->name.c_str());
            continue;
          }
          edge[i].push_back(it->second);
        }
        else
        {
          ECS_ERROR("%s doesn't exist for before %s", before.c_str(), queries[i]->name.c_str());
        }
      }
      for (const string &after : queries[i]->after)
      {
        auto it = nameMap.find(after);
        if (it != nameMap.end())
        {
          if (i == it->second)
          {
            ECS_ERROR("system %s has itself in after", queries[i]->name.c_str());
            continue;
          }
          edge[it->second].push_back(i);
        }
        else
        {
          ECS_ERROR("%s doesn't exist for after %s", after.c_str(), queries[i]->name.c_str());
        }
      }
    }
    for (uint i = 0; i < size; i++)
    {
      if (used[i] == Black)
        dfs(i, edge, used, answer);
    }
    ecs::vector<Description *> rightOrder(size);

    for (uint i = 0; i < size; i++)
      rightOrder[i] = queries[answer[size - 1 - i]];

    queries.swap(rightOrder);
  }

  static bool tagSatisfaction(const ecs::vector<ecs::string> &manager_tags, const ecs::vector<ecs::string> &system_tags)
  {
    for (const auto &requiredTag : system_tags)
    {
      bool found = false;
      for (const auto &possibleTag : manager_tags)
        if (possibleTag == requiredTag)
        {
          found = true;
          break;
        }
      if (!found)
        return false;
    }
    return true;
  }

  template <typename Description>
  void system_sort(const ecs::vector<ecs::string> &applicationTags,
                   const ecs::vector<ecs::unique_ptr<Description>> &all_queries,
                   ecs::vector<Description *> &sorted_queries)
  {
    sorted_queries.clear();
    for (const auto &callable : all_queries)
    {
      if (tagSatisfaction(applicationTags, callable->tags))
        sorted_queries.push_back(callable.get());
    }
    topological_sort(sorted_queries);
  }

  void QueryManager::clearCache()
  {
    for (auto &q : queries)
      q->cache->archetypes.clear();
    for (auto &q : systems)
      for (auto &s : q)
        s->cache->archetypes.clear();
    for (auto &e : events)
      for (auto &q : e)
        q->cache->archetypes.clear();
    for (auto &e : requests)
      for (auto &q : e)
        q->cache->archetypes.clear();
  }

  void QueryManager::invalidate()
  {
    queryInvalidated = true;
    systemsInvalidated = true;
    eventsInvalidated = true;
    requestsInvalidated = true;
  }

  bool QueryManager::requireUpdate() const
  {
    return systemsInvalidated || eventsInvalidated || requestsInvalidated || queryInvalidated;
  }

  void QueryManager::rebuildDependencyGraph()
  {
    if (systemsInvalidated)
    {
      ProfileScope scope("rebuild systems graph");
      ECS_ASSERT(systems.size() == activeSystems.size());
      for (uint i = 0, n = systems.size(); i < n; i++)
        system_sort(tags, systems[i], activeSystems[i]);
      systemsInvalidated = false;
    }
    if (eventsInvalidated)
    {
      ProfileScope scope("rebuild events graph");
      activeEvents.resize(events.size());
      for (uint i = 0, n = events.size(); i < n; i++)
        system_sort(tags, events[i], activeEvents[i]);
      eventsInvalidated = false;
    }
    if (requestsInvalidated)
    {
      ProfileScope scope("rebuild requests graph");
      activeRequests.resize(requests.size());
      for (uint i = 0, n = requests.size(); i < n; i++)
        system_sort(tags, requests[i], activeRequests[i]);
      requestsInvalidated = false;
    }
    queryInvalidated = false;
  }

  void QueryManager::performDefferedEvents()
  {
    ProfileScope scope("performDefferedEvents");
    rebuildDependencyGraph();
    for (int i = 0, n = eventsQueue.size(); i < n; i++)
    {
      eventsQueue.front()();
      eventsQueue.pop();
    }
  }

  void perform_deffered_events()
  {
    get_query_manager().performDefferedEvents();
  }


  static void update_cache(const Archetype &archetype, uint idx, const QueryDescription &desription, QueryCache &cache)
  {
    if (desription.noArchetype)
      return;
    for (const auto &d : desription.requiredComponents)
      if (archetype.findComponentIndex(d) == -1)
        return;
    for (const auto &d : desription.requiredNotComponents)
      if (archetype.findComponentIndex(d) != -1)
        return;

    ecs::vector<int> indexes(desription.arguments.size(), -1);
    for (uint i = 0, n = desription.arguments.size(); i < n; i++)
    {
      const auto &d = desription.arguments[i];
      if (d.isSingleton)
        continue;
      int ind = archetype.findComponentIndex(d);

      if (ind >= 0 || d.optional)
        indexes[i] = ind;
      else
        return;
    }
    cache.archetypes.emplace(idx, std::move(indexes));
  }

  void QueryManager::addArchetypeToCache(uint archetype_idx)
  {
    rebuildDependencyGraph();
    const Archetype &archetype = get_archetype_manager().archetypes[archetype_idx];
    for (auto &q : queries)
      update_cache(archetype, archetype_idx, *q, *q->cache);
    for (auto &s : activeSystems)
      for (auto &q : s)
      update_cache(archetype, archetype_idx, *q, *q->cache);
    for (auto &e : activeEvents)
      for (auto &q : e)
        update_cache(archetype, archetype_idx, *q, *q->cache);
    for (auto &e : activeRequests)
      for (auto &q : e)
        update_cache(archetype, archetype_idx, *q, *q->cache);
  }

  void update_cache(QueryDescription &desription)
  {
    if (desription.noArchetype)
      return;
    uint idx = 0;
    for (const Archetype &archetype : get_archetype_manager().archetypes)
    {
      update_cache(archetype, idx, desription, *desription.cache);
      idx++;
    }
  }

  void QueryManager::sendEventImmediate(const ecs::Event &event, event_t event_id) const
  {
    if (activeEvents.size() <= event_id)
      return;
    for (const EventDescription *descr : activeEvents[event_id])
    {
      ProfileScope scope(descr->name.c_str());
      descr->broadcastEventHandler(event);
    }
  }

  void QueryManager::sendEventImmediate(EntityId eid, const ecs::Event &event, event_t event_id) const
  {
    if (activeEvents.size() <= event_id)
      return;
    for (const EventDescription *descr : activeEvents[event_id])
    {
      ProfileScope scope(descr->name.c_str());
      descr->unicastEventHandler(eid, event);
    }
  }

  void QueryManager::sendRequest(ecs::Request &request, request_t request_id) const
  {
    if (activeRequests.size() <= request_id)
      return;

    for (const RequestDescription *descr : activeRequests[request_id])
    {
      ProfileScope scope(descr->name.c_str());
      descr->broadcastRequestHandler(request);
    }
  }

  void QueryManager::sendRequest(EntityId eid, ecs::Request &request, request_t request_id) const
  {
    if (activeRequests.size() <= request_id)
      return;
    for (const RequestDescription *descr : activeRequests[request_id])
    {
      ProfileScope scope(descr->name.c_str());
      descr->unicastRequestHandler(eid, request);
    }
  }

  stage_id QueryManager::findStageId(const char *stage_name) const
  {
    auto it = stagesMap.find_as(stage_name);
    if (it != stagesMap.end())
      return it->second;
    return -1u;
  }

  void QueryManager::performStage(const char *stage) const
  {
    performStage(findStageId(stage));
  }

  void QueryManager::performStage(stage_id stage) const
  {
    if (stage < activeSystems.size())
    {
      if (activeSystems[stage].empty())
        return;
      ProfileScope scope(activeSystems[stage][0]->stage.c_str());

      ECS_ASSERT_RETURN(!requireUpdate(), );
      for (auto *system : activeSystems[stage])
      {
        ProfileScope scope(system->name.c_str());
        system->system();
      }
    }
  }

  stage_id find_stage(const char *stage)
  {
    return get_query_manager().findStageId(stage);
  }

  void perform_stage(const char *stage)
  {
    get_query_manager().performStage(stage);
  }

  void perform_stage(stage_id stage)
  {
    get_query_manager().performStage(stage);
  }

  void set_system_tags(const ecs::vector<ecs::string> &tags)
  {
    get_query_manager().invalidate();
    get_query_manager().tags = tags;
  }

  void set_system_tags(ecs::vector<ecs::string> &&tags)
  {
    get_query_manager().invalidate();
    get_query_manager().tags = std::move(tags);
  }
}