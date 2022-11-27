
#include "graph.h"

struct Object
{
};

struct Node
{
  Object *object = nullptr;
  std::string name;
  EdgeList<int> edges;
  unsigned distance = -1u;
  unsigned parent = 0;
};

static std::map<std::string, int> nodeNameToId;
static std::vector<Node> nodes;

void create_nodes(const std::vector<Edge> &edges)
{

  nodeNameToId = get_graph_map<int>(edges, [&](const std::string &name)
                                    { nodes.emplace_back().name = name; return nodes.size() - 1; });

  for (const Edge &e : edges)
  {
    int from = nodeNameToId[e.from];
    int to = nodeNameToId[e.to];
    nodes[from].edges.push_back({to, e.dist});
    nodes[to].edges.push_back({from, e.dist});
  }
}

std::vector<std::vector<std::pair<std::string, int>>> get_edges()
{
  std::vector<std::vector<std::pair<std::string, int>>> e;

  for (auto &node : nodes)
  {
    auto &edges = e.emplace_back();
    for (auto &id : node.edges)
    {
      edges.push_back({nodes[id.first].name, id.second});
    }
  }
  return e;
}

unsigned get_dist(const std::string &node)
{
  return nodes[nodeNameToId[node]].distance;
}

Path<unsigned> dijkstra(const std::string &start_node, const std::string &end_node)
{
  unsigned start = nodeNameToId[start_node];
  unsigned end = nodeNameToId[end_node];
  nodes[start].distance = 0;
  std::set<std::pair<unsigned, unsigned>> nearestQueue;
  nearestQueue.insert({0u, start});
  while (!nearestQueue.empty())
  {
    unsigned v = nearestQueue.begin()->second;
    nearestQueue.erase(nearestQueue.begin());

    for (const auto &[to, len] : nodes[v].edges)
    {
      if (nodes[v].distance + len < nodes[to].distance)
      {
        nearestQueue.erase({nodes[to].distance, to});
        nodes[to].distance = nodes[v].distance + len;
        nodes[to].parent = v;
        nearestQueue.insert({nodes[to].distance, to});
      }
    }
  }
  std::vector<unsigned> path;
  if (nodes[end].distance == -1u)
    return {path, -1u};

  unsigned e = end;
  while (e != start)
  {
    path.push_back(e);
    e = nodes[e].parent;
  }
  path.push_back(e);

  std::reverse(path.begin(), path.end());
  return {path, nodes[end].distance};
}