#include "graph.h"

#include <ecs/type_registration.h>


ECS_TYPE_REGISTRATION(std::string, "string", ecs::DefaultType)
ECS_TYPE_REGISTRATION(EdgeList<ecs::EntityId>, "EdgeList", ecs::DefaultType)

static std::map<std::string, ecs::EntityId> nodeNameToId;
struct cmp
{
  bool operator()(std::pair<unsigned, ecs::EntityId> a, std::pair<unsigned, ecs::EntityId> b) const
  {
    if (a.first != b.first)
      return a.first < b.first;
    return a.second.description < b.second.description;
  }
};

template <typename C>
static void get_node_edges(ecs::EntityId, C);
template <typename C>
static void get_node_name(ecs::EntityId, C);
template <typename C>
static void iterate_nodes(C);

void create_ecs_nodes(const std::vector<Edge> &edges)
{
  auto prefabId = ecs::create_entity_prefab(ecs::EntityPrefab{
      "node_object",
      {
          {"name", std::string()},
          {"edges", EdgeList<ecs::EntityId>()},
          {"distance", -1u},
          {"parents_node", ecs::EntityId()},
      },
      ecs::SizePolicy::Thousands});
  nodeNameToId = get_graph_map<ecs::EntityId>(edges, [&](const std::string &name)
                                              { return ecs::create_entity_immediate(prefabId, {{"name", name}}); });

  for (const Edge &e : edges)
  {
    ecs::EntityId from = nodeNameToId[e.from];
    ecs::EntityId to = nodeNameToId[e.to];

    QUERY()
    get_node_edges(from, [&](EdgeList<ecs::EntityId> &edges)
                   { edges.push_back({to, e.dist}); });
    get_node_edges(to, [&](EdgeList<ecs::EntityId> &edges)
                   { edges.push_back({from, e.dist}); });
  }
}

std::vector<std::vector<std::pair<std::string, int>>> get_ecs_edges()
{
  std::vector<std::vector<std::pair<std::string, int>>> e;

  QUERY()
  iterate_nodes([&](const EdgeList<ecs::EntityId> &edges)
                {
    auto &edgesNames = e.emplace_back();
 
    for (auto &[eid, l] : edges)
    {
      int len = l;
      QUERY()
      get_node_name(eid, [&](const std::string &name)
                    { edgesNames.push_back({name, len}); });
    } });
  return e;
}

template <typename C>
static void get_node_dist(ecs::EntityId, C);
template <typename C>
static void get_node_parent_dist(ecs::EntityId, C);
template <typename C>
static void get_node_parent(ecs::EntityId, C);
template <typename C>
static void get_node_edges_dist(ecs::EntityId, C);

unsigned get_dist(ecs::EntityId eid)
{
  unsigned dist = -1u;
  QUERY()
  get_node_dist(eid, [&](unsigned &distance)
                { dist = distance; });
  return dist;
}

unsigned get_ecs_dist(const std::string &node)
{
  return get_dist(nodeNameToId[node]);
}

Path<ecs::EntityId> ecs_dijkstra(const std::string &start_node, const std::string &end_node)
{
  ecs::EntityId start = nodeNameToId[start_node];
  ecs::EntityId end = nodeNameToId[end_node];

  get_node_dist(start, [](unsigned &distance)
                { distance = 0; });

  std::set<std::pair<unsigned, ecs::EntityId>, cmp> nearestQueue;
  nearestQueue.insert({0u, start});
  while (!nearestQueue.empty())
  {
    ecs::EntityId v = nearestQueue.begin()->second;
    nearestQueue.erase(nearestQueue.begin());

    QUERY()
    get_node_edges_dist(v, [&](const EdgeList<ecs::EntityId> &edges, unsigned distance)
                        {
      unsigned distanceFrom = distance;
      for (const auto &[to, len] : edges)
      {
        int length = len;
        ecs::EntityId toEid = to;
        QUERY()get_node_parent_dist(to, [&](unsigned &distance, ecs::EntityId &parents_node)
        {
          if (distanceFrom + length < distance)
          {
            nearestQueue.erase({distance, toEid});
            distance = distanceFrom + length;
            parents_node = v;
            nearestQueue.insert({distance, toEid});
          }
        });
      } });
  }
  std::vector<ecs::EntityId> path;

  if (get_dist(end) == -1u)
    return {path, -1u};

  ecs::EntityId e = end;
  while (e != start)
  {
    path.push_back(e);

    QUERY()
    get_node_parent(e, [&](ecs::EntityId parents_node)
                    { e = parents_node; });
  }
  path.push_back(e);

  std::reverse(path.begin(), path.end());
  return {path, get_dist(end)};
}