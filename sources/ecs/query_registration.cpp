#include <ecs/query_manager.h>

namespace ecs
{
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
    if (event_id >= query_manager.events.size())
      query_manager.events.resize(event_id + 1);

    update_cache(query_manager.events[event_id].emplace_back(std::move(event)));
    query_manager.eventsInvalidated = true;
  }
  void register_request(RequestDescription &&request, request_t request_id)
  {
    if (request_id >= query_manager.requests.size())
      query_manager.requests.resize(request_id + 1);

    update_cache(query_manager.requests[request_id].emplace_back(std::move(request)));

    query_manager.requestsInvalidated = true;
  }
}