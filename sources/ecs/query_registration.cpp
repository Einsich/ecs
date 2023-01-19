#include <ecs/query_manager.h>
#include <ecs/type_annotation.h>

namespace ecs
{
  uint get_next_query_id()
  {
    static uint queryId = 0;
    return ++queryId;
  }

  void QueryDescription::validate() const
  {
    const auto &types = get_all_registered_types();
    for (const ArgumentDescription &descr : arguments)
    {
      if (descr.isSingleton)
        continue;
      const auto &type = types[descr.typeIndex];
      if (descr.accessType == AccessType::Copy)
      {
        if (!type.typeFabric->trivialCopy)
        {
          ECS_ERROR("%s in %s copy component %s %s with not trivial copy ctor",
                    file.c_str(), name.c_str(), type.name.c_str(), descr.name.c_str());
        }
        else if (type.sizeOf > sizeof(float) * 4)
        {
          ECS_ERROR("%s in %s copy component %s %s with big sizeof == %d",
                    file.c_str(), name.c_str(), type.name.c_str(), descr.name.c_str(), type.sizeOf);
        }
      }
    }
  }
  static QueryManager query_manager;

  QueryManager &get_query_manager()
  {
    return query_manager;
  }
  void update_cache(QueryDescription &desription);

  QueryHandle register_query(ecs::string &&file,
                             ecs::string &&name,
                             QueryCache *cache,
                             ecs::vector<ArgumentDescription> &&arguments,
                             ecs::vector<ComponentDescription> &&required_components,
                             ecs::vector<ComponentDescription> &&required_not_components)
  {
    QueryHandle h;
    auto &ptr = query_manager.queries.emplace_back(ecs::make_unique<ecs::QueryDescription>(
        std::move(file),
        std::move(name),
        cache,
        std::move(arguments),
        std::move(required_components),
        std::move(required_not_components)));
    update_cache(*ptr.get());
    query_manager.queryInvalidated = true;
    h.uniqueId = ptr->uniqueId;
    query_manager.allQueryMap[h.uniqueId] = ptr.get();
    return h;
  }

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
                               SystemDescription::SystemHandler system)
  {
    SystemHandle h;
    auto it = query_manager.stagesMap.find(stage);
    if (it == query_manager.stagesMap.end())
      it = query_manager.stagesMap.insert({stage, query_manager.stagesMap.size()}).first;
    uint k = it->second;
    if (k >= query_manager.systems.size())
    {
      query_manager.systems.resize(k + 1);
      query_manager.activeSystems.resize(k + 1);
    }

    auto &ptr = query_manager.systems[k].emplace_back(ecs::make_unique<ecs::SystemDescription>(
        std::move(file),
        std::move(name),
        cache,
        std::move(arguments),
        std::move(required_components),
        std::move(required_not_components),
        std::move(stage),
        std::move(before),
        std::move(after),
        std::move(tags),
        system));
    update_cache(*ptr.get());
    query_manager.systemsInvalidated = true;

    h.stageId = k;
    h.uniqueId = ptr->uniqueId;
    query_manager.allQueryMap[h.uniqueId] = ptr.get();
    return h;
  }

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
                             event_t event_id)
  {
    EventHandle h;
    if (event_id == -1u)
    {
      ECS_ERROR("event handler %s use unregistered event", name.c_str());
      return h;
    }
    if (event_id >= query_manager.events.size())
      query_manager.events.resize(event_id + 1);

    auto &ptr = query_manager.events[event_id].emplace_back(ecs::make_unique<ecs::EventDescription>(
        std::move(file),
        std::move(name),
        cache,
        std::move(arguments),
        std::move(required_components),
        std::move(required_not_components),
        std::move(before),
        std::move(after),
        std::move(tags),
        broadcast_event,
        unicast_event));
    update_cache(*ptr.get());
    query_manager.eventsInvalidated = true;
    h.typeId = event_id;
    h.uniqueId = ptr->uniqueId;
    query_manager.allQueryMap[h.uniqueId] = ptr.get();
    return h;
  }

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
                                 request_t request_id)
  {
    RequestHandle h;
    if (request_id == -1u)
    {
      ECS_ERROR("request handler %s use unregistered request", name.c_str());
      return h;
    }
    if (request_id >= query_manager.requests.size())
      query_manager.requests.resize(request_id + 1);

    auto &ptr = query_manager.requests[request_id].emplace_back(ecs::make_unique<ecs::RequestDescription>(
        std::move(file),
        std::move(name),
        cache,
        std::move(arguments),
        std::move(required_components),
        std::move(required_not_components),
        std::move(before),
        std::move(after),
        std::move(tags),
        broadcast_request,
        unicast_request));
    update_cache(*ptr.get());
    query_manager.requestsInvalidated = true;

    h.typeId = request_id;
    h.uniqueId = ptr->uniqueId;
    query_manager.allQueryMap[h.uniqueId] = ptr.get();
    return h;
  }

  template<typename T>
  static bool remove_query(uint unique_id, bool free_cache, ecs::vector<T> &queries, bool &invalidated)
  {
    for (uint i = 0, n = queries.size(); i < n; i++)
    {
      if (queries[i]->uniqueId == unique_id)
      {
        query_manager.allQueryMap.erase(unique_id);
        if (free_cache)
        {
          delete queries[i]->cache;
        }
        queries.erase(queries.begin() + i);
        invalidated = true;
        return true;
      }
    }
    return false;
  }

  bool remove_query(QueryHandle handle, bool free_cache)
  {
    return remove_query(handle.uniqueId, free_cache, query_manager.queries, query_manager.queryInvalidated);
  }

  bool remove_system(SystemHandle handle, bool free_cache)
  {
    if (handle.stageId < query_manager.systems.size())
    {
      return remove_query(handle.uniqueId, free_cache, query_manager.systems[handle.stageId], query_manager.systemsInvalidated);
    }
    return false;
  }

  bool remove_event(EventHandle handle, bool free_cache)
  {
    if (handle.typeId < query_manager.events.size())
    {
      return remove_query(handle.uniqueId, free_cache, query_manager.events[handle.typeId], query_manager.eventsInvalidated);
    }
    return false;
  }

  bool remove_request(RequestHandle handle, bool free_cache)
  {
    if (handle.typeId < query_manager.requests.size())
    {
      return remove_query(handle.uniqueId, free_cache, query_manager.requests[handle.typeId], query_manager.requestsInvalidated);
    }
    return false;
  }

  struct SystemRegistrationList
  {
    ecs::vector<void (*)()> registration;
  };

  static SystemRegistrationList &get_registration_list()
  {
    static SystemRegistrationList systemREgistrationList;
    return systemREgistrationList;
  }

  void file_registration(void (*pull_function)())
  {
    get_registration_list().registration.push_back(pull_function);
  }

  void register_all_pulled_files()
  {
    for (auto pull : get_registration_list().registration)
      pull();
  }

}