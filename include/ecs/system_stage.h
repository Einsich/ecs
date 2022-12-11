#pragma once
#include <functional>

namespace ecs
{
  struct SystemStage
  {
    using SyncPoint = std::function<void()>;
    ecs::string name;
    SyncPoint begin, end;
    SystemStage(const char *stage_name, SyncPoint begin_stage, SyncPoint end_stage)
        : name(stage_name), begin(begin_stage), end(end_stage)
    {
    }
  };
}