#pragma once
#include <ecs/ecs_std.h>
#include <ecs/event.h>

namespace ecs
{
  template <typename T>
  void register_event(const char *name, bool trivial_ctor)
  {
    ECS_ASSERT(EventIndex<T>::value == -1);
    extern int get_next_event_index();
    extern void register_event(int, EventTypeDescription);
    EventIndex<T>::value = get_next_event_index();
    register_event(EventIndex<T>::value, EventTypeDescription{name, sizeof(T), trivial_ctor});
  }
  template <typename T>
  struct EventRegistrationHelper
  {
    EventRegistrationHelper(const char *name, bool trivial_ctor = false)
    {
      register_event<T>(name, trivial_ctor);
    }
  };
}

#define ECS_EVENT_REGISTRATION(TYPE) \
  static ecs::EventRegistrationHelper<TYPE> __CONCAT__(eventRegistrator, __COUNTER__)(#TYPE);\
