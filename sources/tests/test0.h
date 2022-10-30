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
