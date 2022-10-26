#include <ecs/ecs.h>

template <typename C>
static void test_query(C);

SYSTEM()
test(const float &x, const float &y, const int &z)
{
  printf("hi %f %f %d\n", x, y, z);

  bool one_time = false;

  QUERY()
  test_query([&](int &z, float &y)
             { if (one_time) return; y+=10; z-=10; one_time = true; });

  printf("bye %f %f %d\n", x, y, z);
}
struct MyEvent : ecs::Event
{
  int x;
};

EVENT()
test_event(const MyEvent &e, float y)
{
  printf("test_event %d %f\n", e.x, y);
}

struct MyRequest : ecs::Request
{
  int count = 0;
};
 
REQUEST()
test_request(MyRequest &r) // requare float x
{
  r.count += 1;
}