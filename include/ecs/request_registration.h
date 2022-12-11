#pragma once
#include <ecs/request.h>

namespace ecs
{

  template <typename T>
  void register_request(const char *name)
  {
    ECS_ASSERT(RequestIndex<T>::value == -1);
    extern int get_next_request_index();
    extern void register_request(int, RequestTypeDescription);
    RequestIndex<T>::value = get_next_request_index();
    register_request(RequestIndex<T>::value, RequestTypeDescription{name});
  }
  template <typename T>
  struct RequestRegistrationHelper
  {
    RequestRegistrationHelper(const char *name)
    {
      register_request<T>(name);
    }
  };
}

#define ECS_REQUEST_REGISTRATION(TYPE) \
  static ecs::RequestRegistrationHelper<TYPE> __CONCAT__(requestRegistrator, __COUNTER__)(#TYPE);\
