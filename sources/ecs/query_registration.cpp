#include <ecs/query_manager.h>
#include <ecs/type_annotation.h>

namespace ecs
{

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
        if (type.copyConstructor != nullptr)
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

  void register_query(QueryDescription &&query)
  {
    update_cache(query_manager.queries.emplace_back(std::move(query)));
    query_manager.queryInvalidated = true;
  }
  void register_system(SystemDescription &&system)
  {
    update_cache(query_manager.addSystem(std::move(system)));
    query_manager.systemsInvalidated = true;
  }
  void register_event(EventDescription &&event, event_t event_id)
  {
    if (event_id == -1u)
    {
      ECS_ERROR("event handler %s use unregistered event", event.name.c_str());
      return;
    }
    if (event_id >= query_manager.events.size())
      query_manager.events.resize(event_id + 1);

    update_cache(query_manager.events[event_id].emplace_back(std::move(event)));
    query_manager.eventsInvalidated = true;
  }
  void register_request(RequestDescription &&request, request_t request_id)
  {
    if (request_id == -1u)
    {
      ECS_ERROR("request handler %s use unregistered request", request.name.c_str());
      return;
    }
    if (request_id >= query_manager.requests.size())
      query_manager.requests.resize(request_id + 1);

    update_cache(query_manager.requests[request_id].emplace_back(std::move(request)));

    query_manager.requestsInvalidated = true;
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