#include <ecs/ecs.h>
#include <ecs/time_profiler.h>

#include <iostream>
#include <fstream>
#include <map>
#include "graph.h"

void create_nodes(const std::vector<Edge> &edges);
Path<unsigned> dijkstra(const std::string &start_node, const std::string &end_node);
unsigned get_dist(const std::string &node);
std::vector<std::vector<std::pair<std::string, int>>> get_edges();

void create_ecs_nodes(const std::vector<Edge> &edges);
Path<ecs::EntityId> ecs_dijkstra(const std::string &start_node, const std::string &end_node);
unsigned get_ecs_dist(const std::string &node);
std::vector<std::vector<std::pair<std::string, int>>> get_ecs_edges();

static int perf_test(const char *label, void (*test)())
{
  TimeProfile a(label);
  test();
  return a.get_time();
}

int main()
{
  ecs::init();
  ecs::init_singletones();
  ecs::pull_registered_files();
  int n, m;

  std::vector<Edge> edges;
  if (1)
  {
    std::ifstream graph;
    // Nodes count, edges count
    // List of edges (From, To)
    graph.open("sources/tests/test7/roadNet-CA.txt", std::ios::in);

    graph >> n >> m;
    edges.resize(m);

    for (Edge &e : edges)
    {
      graph >> e.from >> e.to >> e.dist;
    }
    graph.close();
  }
  else
  {
    n = 4;
    m = 4;
    edges.push_back(Edge{"0", "3", 1});
    edges.push_back(Edge{"1", "3", 1});
    edges.push_back(Edge{"2", "3", 3});
    edges.push_back(Edge{"1", "2", 1});
  }

  printf("%d %d\n", n, m);

  {
    TimeProfile a("oop nodes creation");
    create_nodes(edges);
  }

  {
    TimeProfile a("ecs nodes creation");
    create_ecs_nodes(edges);
  }

  {
    TimeProfile a("oop path finding");
    auto path = dijkstra("0", "1966010");
    // for (auto x : path.first)
    //   printf("\"%s\"[%d], ", nodes[x].name.c_str(), x);
    printf("\npath %d\n", path.second);
  }
  {
    TimeProfile a("ecs path finding");
    auto path = ecs_dijkstra("0", "1966010");
    // for (auto x : path.first)
    //   printf("\"%s\"[%d], ", nodes[x].name.c_str(), x);
    printf("\npath %d\n", path.second);
  }

  for (Edge &e : edges)
  {
    if (get_dist(e.from) != get_ecs_dist(e.from))
    {
      printf("wrong node %s\n", e.from.c_str());
      break;
    }
  }
  int N = 3000;

  struct Test
  {
    ecs::string name;
    std::function<int()> benchmark;
    int sumOfTime, maxTime;
  };

#define TESTS \
  TEST(ecs_query_iteration)

#define TEST(test) void test##_implementation();
  TESTS
#undef TEST

#define TEST(test) {#test, []() { return perf_test(#test, test##_implementation); }, 0, 0},

  ecs::vector<Test> tests = {
      TESTS};
#undef TEST

  ecs::vector<int> order;
  int k = tests.size();
  for (int i = 0; i < k; i++)
  {
    order.push_back(i);
  }
  n = 4 * 3 * 2;

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < k; j++)
    {
      Test &test = tests[order[j]];
      int t = test.benchmark();
      test.sumOfTime += t;
      test.maxTime = test.maxTime < t ? t : test.maxTime;
    }
    std::next_permutation(order.begin(), order.end());
  }

  for (const Test &test : tests)
  {
    ECS_LOG("spent avg %d, max %d us. in %s\n", test.sumOfTime / n, test.maxTime, test.name.c_str());
  }
  std::cout << std::endl;
  ecs::destroy_all_entities();
  ecs::destroy_sinletons();
  std::fflush(stdout);
  return 0;
}

template <typename C>
static void query(C);

SYSTEM()
ecs_query_iteration()
{
  QUERY()
  query([&]() {});
}
