#include <ecs/query_manager.h>
#include <ecs/ska/flat_hash_map.hpp>
#include <ecs/archetype_manager.h>

namespace ecs
{

  static void dfs(uint v, const ecs::vector<ecs::vector<uint>> &edges, ecs::vector<bool> &used, ecs::vector<uint> &answer)
  {
    used[v] = true;
    for (uint to : edges[v])
    {
      if (!used[to])
        dfs(to, edges, used, answer);
    }
    answer.push_back(v);
  }

  template <typename Description>
  static void topological_sort(ecs::vector<Description *> &queries)
  {
    auto size = queries.size();
    ecs::vector<ecs::vector<uint>> edge(size);
    ecs::vector<bool> used(size, false);
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
          edge[i].push_back(it->second);
        }
        else
        {
          ECS_ERROR("%s doesn't exist for before %s\n", before.c_str(), queries[i]->name.c_str());
        }
      }
      for (const string &after : queries[i]->after)
      {
        auto it = nameMap.find(after);
        if (it != nameMap.end())
        {
          edge[it->second].push_back(i);
        }
        else
        {
          ECS_ERROR("%s doesn't exist for after %s\n", after.c_str(), queries[i]->name.c_str());
        }
      }
    }
    for (uint i = 0; i < size; i++)
    {
      if (!used[i])
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
                   const ecs::vector<Description> &all_queries,
                   ecs::vector<Description *> &sorted_queries)
  {
    sorted_queries.clear();
    for (const Description &callable : all_queries)
    {
      if (tagSatisfaction(applicationTags, callable.tags))
        sorted_queries.push_back((Description *)&callable);
    }
    topological_sort(sorted_queries);
  }

  void QueryManager::clearCache()
  {
    for (auto &q : queries)
      q.cache->archetypes.clear();
    for (auto &q : systems)
      q.cache->archetypes.clear();
    for (auto &e : events)
      for (auto &q : e)
        q.cache->archetypes.clear();
    for (auto &e : requests)
      for (auto &q : e)
        q.cache->archetypes.clear();
  }
  void QueryManager::rebuildDependencyGraph()
  {
    if (systemsInvalidated)
    {
      system_sort(tags, systems, activeSystems);
      systemsInvalidated = false;
    }
    if (eventsInvalidated)
    {
      activeEvents.resize(events.size());
      for (uint i = 0, n = events.size(); i < n; i++)
        system_sort(tags, events[i], activeEvents[i]);
      eventsInvalidated = false;
    }
    if (requestsInvalidated)
    {
      activeRequests.resize(requests.size());
      for (uint i = 0, n = requests.size(); i < n; i++)
        system_sort(tags, requests[i], activeRequests[i]);
      requestsInvalidated = false;
    }
  }

  void QueryManager::update()
  {
    rebuildDependencyGraph();
    for (int i = 0, n = eventsQueue.size(); i < n; i++)
    {
      eventsQueue.front()();
      eventsQueue.pop();
    }
  }

  void update_query_manager()
  {
    get_query_manager().update();
  }

  void perform_systems()
  {
    for (auto *system : get_query_manager().activeSystems)
    {
      system->system();
    }
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
      int ind = archetype.findComponentIndex(d);

      if (ind >= 0 || d.optional)
        indexes[i] = ind;
      else
        return;
    }
    auto &cachedArchetype = cache.archetypes.emplace_back();
    cachedArchetype.archetypeIndex = idx;
    cachedArchetype.componentIndexes = std::move(indexes);
  }

  void QueryManager::addArchetypeToCache(uint archetype_idx)
  {
    update();
    const Archetype &archetype = get_archetype_manager().archetypes[archetype_idx];
    for (auto &q : queries)
      update_cache(archetype, archetype_idx, q, *q.cache);
    for (auto &q : activeSystems)
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

  void QueryManager::send_event_immediate(const ecs::Event &event, event_t event_id) const
  {
    if (activeEvents.size() <= event_id)
      return;
    for (const EventDescription *descr : activeEvents[event_id])
    {
      descr->broadcastEventHandler(event);
    }
  }

  void QueryManager::send_event_immediate(EntityId eid, const ecs::Event &event, event_t event_id) const
  {
    if (activeEvents.size() <= event_id)
      return;
    for (const EventDescription *descr : activeEvents[event_id])
    {
      descr->unicastEventHandler(eid, event);
    }
  }
  void QueryManager::send_request(ecs::Request &request, request_t request_id) const
  {
    if (activeRequests.size() <= request_id)
      return;

    for (const RequestDescription *descr : activeRequests[request_id])
    {
      descr->broadcastRequestHandler(request);
    }
  }
  void QueryManager::send_request(EntityId eid, ecs::Request &request, request_t request_id) const
  {
    if (activeRequests.size() <= request_id)
      return;
    for (const RequestDescription *descr : activeRequests[request_id])
    {
      descr->unicastRequestHandler(eid, request);
    }
  }
}