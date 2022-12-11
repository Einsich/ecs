#pragma once
#include <ecs/ecs_std.h>

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
}

