#include <ecs/ecs.h>
#include <ecs/type_registration.h>
#include "test0.h"

struct ResourceRequest
{
  const char *name;
};

class ResourceAcquirer
{
public:
  ResourceAcquirer(const char *name)
  {
    printf("created %s\n", name);
  }
  bool await_constructor(void *raw_memory, const ecs::ComponentPrefab &prefab)
  {
    static int cooldown = 0;
    auto name = prefab.get<ResourceRequest>()->name;
    cooldown++;
    printf("await %s %d\n", name, cooldown);
    if (cooldown % 5 != 0)
      return false;

    new (raw_memory) ResourceAcquirer(name);
    return true;
  }
};
ECS_TYPE_REGISTRATION(ResourceAcquirer, "ResourceAcquirer", false, false, false, {})

EVENT()
test_event(const PrepareTest &)
{

  ecs::prefab_id p = ecs::create_entity_prefab(
      {"delayed_creation",
       {{"value", 1.f}, {"resource", ecs::ComponentInitializer<ResourceAcquirer, ResourceRequest>({"null"})}}});

  ecs::create_entity(p, {{"value", 2.f}, {"resource", ecs::ComponentInitializer<ResourceAcquirer, ResourceRequest>({"two"})}});
  ecs::create_entity(p, {{"value", 1.f}, {"resource", ecs::ComponentInitializer<ResourceAcquirer, ResourceRequest>({"one"})}});
}
