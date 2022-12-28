#pragma once
#include <ecs/ecs.h>

namespace ecs
{
  struct ProfileScope
  {
    ProfileScope(const char* label)
    {
      if (ecs_profiler_enabled)
        ecs_profiler_push(label);
    }
    ~ProfileScope()
    {
      if (ecs_profiler_enabled)
        ecs_profiler_pop();
    }
  };
}