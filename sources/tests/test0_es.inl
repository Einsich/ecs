#include <ecs/ecs.h>
#include "test0.h"

template <typename C>
static void test_single_query(ecs::EntityId eid, C c);

SYSTEM()
test(const float &x, const float &y, const int &z, ecs::EntityId eid)
{
  printf("hi %f %f %d\n", x, y, z);
  ecs::uint a, i;
  ecs::EntityState s;
  if (eid.get_info(a, i, s))
  {
    printf("eid %d %d %d\n", a, i, s);
  }
  else
  {
    printf("eid invalid\n");
  }

  QUERY()
  test_single_query(eid, [&](int &z, float &y)
                    { y+=10; z-=10; });

  printf("bye %f %f %d\n", x, y, z);
}

EVENT()
test_event(const MyEvent &e, float y)
{
  printf("test_event %d %f\n", e.x, y);
}

REQUEST()
test_request(MyRequest &r) // requare float x
{
  r.count += 1;
}
