#pragma once

struct MyEvent : ecs::Event
{
  int x;
};

struct MyRequest : ecs::Request
{
  int count = 0;
};
