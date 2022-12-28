#include <ecs/ecs.h>
#include <ecs/type_registration.h>
#include "../tests.h"

void print(ecs::EntityId eid);

struct EntityDependance
{
  ecs::EntityId eid;
};

static bool await_component(const ecs::ComponentPrefab &prefab)
{

  auto eid = prefab.get<ecs::EntityId>();
  if (eid.description && eid.description->state == ecs::EntityState::CreatedNotInited)
    return false;
  ECS_LOG("await");
  print(eid);
  return true;
}

static void await_constructor(void *raw_memory, const ecs::ComponentPrefab &prefab, bool )
{
  auto eid = prefab.get<ecs::EntityId>();
  new (raw_memory) EntityDependance{eid};
}

struct RandomWaiter
{
};
static int globalT = 0;

static bool random_wait(const ecs::ComponentPrefab &prefab)
{
  int time = prefab.get<int>();
  globalT++;
  ECS_LOG("wait %d / %d", globalT, time);

  return globalT >= time;
}

static void rand_await_constructor(void *, const ecs::ComponentPrefab &, bool )
{
}

ECS_TYPE_REGISTRATION(EntityDependance, "EntityDependance", ecs::DefaultType, nullptr, &await_component, &await_constructor)
ECS_TYPE_REGISTRATION(RandomWaiter, "RandomWaiter", ecs::DefaultType, nullptr, &random_wait, &rand_await_constructor)

EVENT()
test_event(const PrepareTest &)
{
  ecs::prefab_id p = ecs::create_entity_prefab(
      {"delayed_creation",
       {{"value", -1},
        {"nextEid", ecs::EntityId()},
        {"resource", ecs::ComponentInitializer<EntityDependance, ecs::EntityId>(ecs::EntityId())},
        {"timer", ecs::ComponentInitializer<RandomWaiter, int>(0)}}});

  ecs::EntityId eid = ecs::create_entity(p, {{"value", 0}});
  eid = ecs::create_entity(
      p,
      {{"value", 1},
       {"nextEid", eid},
       {"resource", ecs::ComponentInitializer<EntityDependance, ecs::EntityId>({eid})},
       {"timer", ecs::ComponentInitializer<RandomWaiter, int>(3)}});
  eid = ecs::create_entity(
      p,
      {{"value", 2},
       {"nextEid", eid},
       {"resource", ecs::ComponentInitializer<EntityDependance, ecs::EntityId>({eid})},
       {"timer", ecs::ComponentInitializer<RandomWaiter, int>(0)}});
  eid = ecs::create_entity(
      p,
      {{"value", 3},
       {"nextEid", eid},
       {"resource", ecs::ComponentInitializer<EntityDependance, ecs::EntityId>({eid})},
       {"timer", ecs::ComponentInitializer<RandomWaiter, int>(1)}});
  eid = ecs::create_entity(
      p,
      {{"value", 4},
       {"nextEid", eid},
       {"resource", ecs::ComponentInitializer<EntityDependance, ecs::EntityId>({eid})},
       {"timer", ecs::ComponentInitializer<RandomWaiter, int>(2)}});
  eid = ecs::create_entity(
      p,
      {{"value", 5},
       {"nextEid", eid},
       {"resource", ecs::ComponentInitializer<EntityDependance, ecs::EntityId>({eid})},
       {"timer", ecs::ComponentInitializer<RandomWaiter, int>(0)}});
}

template <typename C>
static void get_next_node(ecs::EntityId eid, C c);

static void iterate_linked_list(ecs::EntityId head, int idx)
{
  QUERY()
  get_next_node(head,
                [&](ecs::EntityId nextEid, int value)
                {
                  ECS_LOG("value %d [%d]", value, idx);
                  iterate_linked_list(nextEid, idx + 1);
                });
}

EVENT(require = ecs::EntityId nextEid, int value)
test_awaited_creation(const ecs::OnEntityCreated &, int value, ecs::EntityId eid)
{
  if (value != 5)
  {
    return;
  }
  ECS_LOG("[[");
  iterate_linked_list(eid, 0);
  ECS_LOG("]]");
}

EVENT(require = ecs::EntityId nextEid)
test_awaited_delete(const ecs::OnEntityDestroyed &, int value)
{
  ECS_LOG("OnEntityDestroyed %d", value);
}

EVENT(require = ecs::EntityId nextEid)
test_awaited_terminate(const ecs::OnEntityTerminated &, int value)
{
  ECS_LOG("OnEntityTerminated %d", value);
}