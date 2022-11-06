#pragma once

namespace ecs
{
  struct SystemStage
  {
    using SyncPoint = void (*)();
    ecs::string name;
    SyncPoint before, after;
    SystemStage(const char *stage_name, SyncPoint before_stage, SyncPoint after_stage)
        : name(stage_name), before(before_stage), after(after_stage)
    {
    }
  };
}