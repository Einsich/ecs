#include <ecs/ecs.h>
#include "../tests.h"

template <typename C>
static void get_next_node(ecs::EntityId eid, C c);

static void iterate_linked_list(ecs::EntityId head)
{
  QUERY()
  get_next_node(head,
                [&](ecs::EntityId nextEid, float value)
                {
                  printf("value %f\n", value);
                  iterate_linked_list(nextEid);
                });
}

SYSTEM()
no_arguments()
{
  static bool firstTime = true;
  static ecs::EntityId tail;
  if (firstTime)
  {
    firstTime = false;

    printf("no_arguments\n");

    ecs::prefab_id p = ecs::create_entity_prefab(
        ecs::EntityPrefab{"linked_list", {{"nextEid", ecs::EntityId()}, {"value", -1.f}}});

    tail = ecs::create_entity(p, {{"value", 4.f}});
    print(tail);
    tail = ecs::create_entity(p, {{"nextEid", tail}, {"value", 3.f}});
    print(tail);
    tail = ecs::create_entity(p, {{"nextEid", tail}, {"value", 2.f}});
    print(tail);
    tail = ecs::create_entity(p, {{"nextEid", tail}, {"value", 1.f}});
    print(tail);
    tail = ecs::create_entity(p, {{"nextEid", tail}, {"value", 0.f}});
    print(tail);
  }
  printf("[[\n");
  iterate_linked_list(tail);
  printf("]]\n");
}

SYSTEM()
test_deffered_creation(const float value, ecs::EntityId eid, ecs::EntityId nextEid)
{
  printf("value %f\n", value);
  print(eid);
  printf("nextEid\n");
  print(nextEid);
}