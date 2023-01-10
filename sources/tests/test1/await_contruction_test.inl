#include <ecs/ecs.h>
#include <ecs/type_registration.h>
#include "../tests.h"

void print(ecs::EntityId eid);

struct EntityDependance
{
  ecs::EntityId eid;
};

struct DependanceFabric final : ecs::DefaultTypeFabric<EntityDependance>
{
  DependanceFabric(): DefaultTypeFabric(ecs::DefaultType, false, true){}

  void await_contructor(void *raw_memory, const ecs::ComponentPrefab &prefab, bool) const override
  {
    auto eid = prefab.get<ecs::EntityId>();
    new (raw_memory) EntityDependance{eid};
  }

  bool component_awaiter(const ecs::ComponentPrefab &prefab) const override
  {
    auto eid = prefab.get<ecs::EntityId>();
    if (eid.description && eid.description->state == ecs::EntityState::CreatedNotInited)
      return false;
    ECS_LOG("await");
    print(eid);
    return true;
  }
} depFabric;


struct RandomWaiter
{
};
static int globalT = 0;

struct RandomFabric final : ecs::DefaultTypeFabric<EntityDependance>
{
  RandomFabric(): DefaultTypeFabric(ecs::DefaultType, false, true){}

  void await_contructor(void *, const ecs::ComponentPrefab &, bool ) const override
  {
  }

  bool component_awaiter(const ecs::ComponentPrefab &prefab) const override
  {
    int time = prefab.get<int>();
    globalT++;
    ECS_LOG("wait %d / %d", globalT, time);

    return globalT >= time;
  }
} randFabric;


ECS_TYPE_REGISTRATION_WITH_FABRIC(EntityDependance, "EntityDependance", &depFabric)
ECS_TYPE_REGISTRATION_WITH_FABRIC(RandomWaiter, "RandomWaiter", &randFabric)

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