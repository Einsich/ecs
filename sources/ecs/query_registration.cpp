#include <ecs/query_manager.h>
#include <ecs/type_annotation.h>
#include <ecs/system_stage.h>

namespace ecs
{

  void QueryDescription::validate() const
  {
    const auto &types = get_all_registered_types();
    for (const ArgumentDescription &descr : arguments)
    {
      const auto &type = types[descr.typeIndex];
      if (descr.accessType == AccessType::Copy)
      {
        if (type.copyConstructor != nullptr)
        {
          ECS_ERROR("%s in %s copy component %s %s with not trivial copy ctor\n",
                    file.c_str(), name.c_str(), type.name.c_str(), descr.name.c_str());
        }
        else if (type.sizeOf > sizeof(float) * 4)
        {
          ECS_ERROR("%s in %s copy component %s %s with big sizeof == %d\n",
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
    update_cache(query_manager.systems.emplace_back(std::move(system)));
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

  static ecs::QueryCache stubCache;
  static void sync_point_stub()
  {
  }

  void init_stages(const ecs::vector<SystemStage> &stages)
  {
    for (uint i = 0, n = stages.size(); i < n; i++)
    {
      const auto &stage = stages[i];
      ecs::string begin_point = stage.name + "_begin_sync_point";
      ecs::string end_point = stage.name + "_end_sync_point";
      ecs::register_system(ecs::SystemDescription(
          "", begin_point.c_str(), &stubCache, {}, {}, {},
          {end_point},
          {}, {}, stage.begin ? stage.begin : &sync_point_stub));

      ecs::vector<ecs::string> nextStage;
      if (i + 1 < n)
        nextStage.emplace_back(stages[i + 1].name + "_begin_sync_point");

      ecs::register_system(ecs::SystemDescription(
          "", end_point.c_str(), &stubCache, {}, {}, {},
          std::move(nextStage),
          {}, {}, stage.end ? stage.end : &sync_point_stub));
    }
  }
}