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
    ECS_LOG("eid %d %d %d, addr %p", a, i, s, eid.description);
  else
    ECS_LOG("eid invalid");
}