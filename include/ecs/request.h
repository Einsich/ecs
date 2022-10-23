#pragma once

namespace ecs
{

  template <typename T>
  struct RequestIndex
  {
    inline static int value = -1;
  };

  struct RequestTypeDescription
  {
    const ecs::string name;
  };

  int request_name_to_index(const char *name);
  const ecs::vector<RequestTypeDescription> &get_all_registered_requests();

  using request_t = uint;
  struct Request
  {
  };

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
      register_event<T>(name);
    }
  };
}

#define ECS_REQUEST_REGISTRATION(TYPE, NAME) \
  static ecs::RequestRegistrationHelper<TYPE> __CONCAT__(requestRegistrator, __LINE__)(NAME);\
