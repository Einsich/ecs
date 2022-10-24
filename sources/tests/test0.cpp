#include <ecs/a.h>
#include <ecs/ecs.h>

template <typename C>
static void test_query(C);

static void test(const float &x, const float &y, const int &z)
{
  printf("hi %f %f %d\n", x, y, z);

  bool one_time = false;
  test_query([&](int &z, float &y)
             { if (one_time) return; y+=10; z-=10; one_time = true; });

  printf("bye %f %f %d\n", x, y, z);
}

#include <ecs/ecs_perform.h>
ecs::QueryCache cache;

static void test_system()
{
  ecs::perform_system(cache, test);
}

ecs::QueryCache query_cache;
template <typename C>
static void test_query(C c)
{
  ecs::perform_query<int &, float &>(query_cache, c);
}
int main()
{
  ecs::register_system(ecs::SystemDescription(
      "test0.cpp",
      "test_system",
      &cache,
      {{"x", ecs::TypeIndex<float>::value, ecs::AccessType::ReadOnly, false},
       {"y", ecs::TypeIndex<float>::value, ecs::AccessType::ReadOnly, false},
       {"z", ecs::TypeIndex<int>::value, ecs::AccessType::ReadOnly, false}}, // args
      {},                                                                    // req
      {{"w", ecs::TypeIndex<int>::value}},                                   // req_not
      {},                                                                    // before
      {},                                                                    // after
      {},                                                                    // tags
      &test_system));

  ecs::register_query(ecs::QueryDescription(
      "test0.cpp",
      "test_query",
      &query_cache,
      {{"z", ecs::TypeIndex<int>::value, ecs::AccessType::ReadWrite, false},
       {"y", ecs::TypeIndex<float>::value, ecs::AccessType::ReadWrite, false}}, // args
      {},                                                                       // req
      {}                                                                        // req_not
      ));
  ecs::EntityPrefab p("lol", {{"x", 1.f}, {"y", 2.f}, {"z", 3}});

  ecs::create_entity_immediate(p);
  ecs::create_entity_immediate(p, {{"x", 2.f}, {"y", -2.f}});
  ecs::create_entity_immediate(p, {{"x", 3.f}, {"z", -2.f}});
  ecs::create_entity_immediate(p, {{"x", 4.f}, {"x", -4.f}});

  ecs::update_query_manager();
  ecs::perform_systems();

  void events_testing();
  events_testing();
  void requests_testing();
  requests_testing();
  f();
  return 0;
}

struct A
{
};

struct B
{
  char b;
};
struct C
{
  int c;
};

static_assert(ecs::is_zero_sizeof<A> == true);
static_assert(ecs::is_zero_sizeof<B> == false);
static_assert(ecs::is_zero_sizeof<C> == false);

struct MyEvent : ecs::Event
{
  int x;
};

ecs::QueryCache event_cache;

static void test_event(const MyEvent &e, float y)
{
  printf("test_event %d %f\n", e.x, y);
}

static void test_event_handler(const ecs::Event &event)
{
  ecs::perform_event(reinterpret_cast<const MyEvent &>(event), event_cache, test_event);
}

static void test_event_eid_handler(ecs::EntityId /* eid */, const ecs::Event & /* event */)
{
}

void events_testing()
{
  ecs::register_event<MyEvent>("MyEvent", true);
  ecs::register_event(ecs::EventDescription(
                          "test0.cpp",
                          "test_event",
                          &event_cache,
                          {{"y", ecs::TypeIndex<float>::value, ecs::AccessType::ReadOnly, false}}, // args
                          {},                                                                      // req
                          {},                                                                      // req_not
                          {},                                                                      // before
                          {},                                                                      // after
                          {},                                                                      // tags
                          &test_event_handler,
                          &test_event_eid_handler),
                      ecs::EventIndex<MyEvent>::value);

  ecs::update_query_manager(); // after register_event
  MyEvent m;
  m.x = 69;
  ecs::send_event_immediate(m);
  m.x = 96;
  ecs::send_event(m);
  ecs::update_query_manager();
}

struct MyRequest : ecs::Request
{
  int count = 0;
};

ecs::QueryCache request_cache;

static void test_request(MyRequest &r) // requare float x
{
  r.count += 1;
}

static void test_request_handler(ecs::Request &request)
{
  ecs::perform_request(reinterpret_cast<MyRequest &>(request), request_cache, test_request);
}

static void test_request_eid_handler(ecs::EntityId /* eid */, ecs::Request & /* request */)
{
}

void requests_testing()
{
  ecs::register_request<MyRequest>("MyRequest");
  ecs::register_request(ecs::RequestDescription("test0.cpp",
                                                "test_request",
                                                &request_cache,
                                                {},                                    // args
                                                {{"x", ecs::TypeIndex<float>::value}}, // req
                                                {},                                    // req_not
                                                {},                                    // before
                                                {},                                    // after
                                                {},                                    // tags
                                                &test_request_handler,
                                                &test_request_eid_handler),
                        ecs::RequestIndex<MyRequest>::value);

  ecs::update_query_manager(); // after register_request
  MyRequest r;
  ecs::send_request(r);
  printf("request count %d\n", r.count);
}