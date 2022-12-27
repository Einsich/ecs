#pragma once
#include <ecs/event.h>

namespace ecs
{
  struct OnEntityCreated : ecs::Event
  {
  };

  struct OnEntityDestroyed : ecs::Event
  {
  };

  struct OnEntityTerminated : ecs::Event
  {
  };

  struct OnSceneCreated : ecs::Event
  {
  };

  struct OnSceneTerminated : ecs::Event
  {
  };

}