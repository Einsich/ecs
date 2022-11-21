#pragma once

#include <ecs/singleton.h>

struct GlobalTime : ecs::Singleton
{
  float data1[1000];
  float dt;
  float data2[1000];
  GlobalTime() : dt(0.2f) {}
};

inline GlobalTime &get_time()
{
  static GlobalTime t;
  return t;
}