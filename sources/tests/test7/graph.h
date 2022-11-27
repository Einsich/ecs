#pragma once
#include <ecs/ecs.h>
#include <ecs/time_profiler.h>
#include <map>
#include <set>

template <typename T>
using EdgeList = std::vector<std::pair<T, int>>;


struct Edge
{
  std::string from, to;
  int dist;
};

template <typename T, typename C>
std::map<std::string, T> get_graph_map(const std::vector<Edge> &edges, C create_node)
{
  std::map<std::string, T> nodeNameToId;
  for (const Edge &e : edges)
  {
    if (nodeNameToId.find(e.from) == nodeNameToId.end())
    {
      auto nextId = create_node(e.from);
      nodeNameToId.insert({e.from, nextId});
    }
    if (nodeNameToId.find(e.to) == nodeNameToId.end())
    {
      auto nextId = create_node(e.to);
      nodeNameToId.insert({e.to, nextId});
    }
  }
  return nodeNameToId;
}

template <typename T>
using Path = std::pair<std::vector<T>, unsigned>;