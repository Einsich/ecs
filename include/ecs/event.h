#pragma once

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
    virtual ~Event()
    {
    }
  };

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
      register_event<T>(name, user_functions);
    }
  };
}

#define ECS_EVENT_REGISTRATION(TYPE, NAME, TRIVIAL_CTOR) \
  static ecs::EventRegistrationHelper<TYPE> __CONCAT__(eventRegistrator, __LINE__)(NAME, TRIVIAL_CTOR);\
