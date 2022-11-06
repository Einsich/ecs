#pragma once

struct PrepareTest : ecs::Event
{
};

struct MyEvent : ecs::Event
{
  int x;
};

struct MyRequest : ecs::Request
{
  int count = 0;
};

inline void print(ecs::EntityId eid)
{
  ecs::uint a, i;
  ecs::EntityState s;
  if (eid.get_info(a, i, s))
    printf("eid %d %d %d, addr %p\n", a, i, s, eid.description);
  else
    printf("eid invalid\n");
}