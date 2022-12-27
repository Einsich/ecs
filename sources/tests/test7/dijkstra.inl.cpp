#include "dijkstra.inl"
#include <ecs/ecs_perform.h>
//Code-generator production

static ecs::QueryCache iterate_nodes__cache__;

static ecs::QueryCache get_node_edges__cache__;

static ecs::QueryCache get_node_name__cache__;

static ecs::QueryCache get_node_dist__cache__;

static ecs::QueryCache get_node_edges_dist__cache__;

static ecs::QueryCache get_node_parent_dist__cache__;

static ecs::QueryCache get_node_parent__cache__;

template<typename Callable>
static void iterate_nodes(Callable lambda)
{
  ecs::perform_query<const EdgeList<ecs::EntityId>&>(iterate_nodes__cache__, lambda);
}

template<typename Callable>
static void get_node_edges(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<EdgeList<ecs::EntityId>&>(get_node_edges__cache__, eid, lambda);
}

template<typename Callable>
static void get_node_name(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<const std::string&>(get_node_name__cache__, eid, lambda);
}

template<typename Callable>
static void get_node_dist(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<unsigned&>(get_node_dist__cache__, eid, lambda);
}

template<typename Callable>
static void get_node_edges_dist(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<const EdgeList<ecs::EntityId>&, unsigned>(get_node_edges_dist__cache__, eid, lambda);
}

template<typename Callable>
static void get_node_parent_dist(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<unsigned&, ecs::EntityId&>(get_node_parent_dist__cache__, eid, lambda);
}

template<typename Callable>
static void get_node_parent(ecs::EntityId eid, Callable lambda)
{
  ecs::perform_query<ecs::EntityId>(get_node_parent__cache__, eid, lambda);
}

static void registration_pull_dijkstra()
{
  ecs::register_query(ecs::QueryDescription(
  "",
  "iterate_nodes",
  &iterate_nodes__cache__,
  {
    {"edges", ecs::get_type_index<EdgeList<ecs::EntityId>>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<EdgeList<ecs::EntityId>>()}
  },
  {},
  {}
  ));

  ecs::register_query(ecs::QueryDescription(
  "",
  "get_node_edges",
  &get_node_edges__cache__,
  {
    {"edges", ecs::get_type_index<EdgeList<ecs::EntityId>>(), ecs::AccessType::ReadWrite, false, ecs::is_singleton<EdgeList<ecs::EntityId>>()}
  },
  {},
  {}
  ));

  ecs::register_query(ecs::QueryDescription(
  "",
  "get_node_name",
  &get_node_name__cache__,
  {
    {"name", ecs::get_type_index<std::string>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<std::string>()}
  },
  {},
  {}
  ));

  ecs::register_query(ecs::QueryDescription(
  "",
  "get_node_dist",
  &get_node_dist__cache__,
  {
    {"distance", ecs::get_type_index<unsigned>(), ecs::AccessType::ReadWrite, false, ecs::is_singleton<unsigned>()}
  },
  {},
  {}
  ));

  ecs::register_query(ecs::QueryDescription(
  "",
  "get_node_edges_dist",
  &get_node_edges_dist__cache__,
  {
    {"edges", ecs::get_type_index<EdgeList<ecs::EntityId>>(), ecs::AccessType::ReadOnly, false, ecs::is_singleton<EdgeList<ecs::EntityId>>()},
    {"distance", ecs::get_type_index<unsigned>(), ecs::AccessType::Copy, false, ecs::is_singleton<unsigned>()}
  },
  {},
  {}
  ));

  ecs::register_query(ecs::QueryDescription(
  "",
  "get_node_parent_dist",
  &get_node_parent_dist__cache__,
  {
    {"distance", ecs::get_type_index<unsigned>(), ecs::AccessType::ReadWrite, false, ecs::is_singleton<unsigned>()},
    {"parents_node", ecs::get_type_index<ecs::EntityId>(), ecs::AccessType::ReadWrite, false, ecs::is_singleton<ecs::EntityId>()}
  },
  {},
  {}
  ));

  ecs::register_query(ecs::QueryDescription(
  "",
  "get_node_parent",
  &get_node_parent__cache__,
  {
    {"parents_node", ecs::get_type_index<ecs::EntityId>(), ecs::AccessType::Copy, false, ecs::is_singleton<ecs::EntityId>()}
  },
  {},
  {}
  ));

}
ECS_FILE_REGISTRATION(&registration_pull_dijkstra)
ECS_PULL_DEFINITION(variable_pull_dijkstra)
