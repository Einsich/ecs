#pragma once
#include <ecs/ecs_std.h>

namespace ecs
{

  template <typename T>
  struct EventIndex
  {
    inline static int value = -1;
  };

  struct EventTypeDescription
  {
    const ecs::string name;
    const uint sizeOf;
    const bool trivialDestructor;
  };

  int event_name_to_index(const char *name);
  const ecs::vector<EventTypeDescription> &get_all_registered_events();

  using event_t = uint;
  struct Event
  {
  };
}

