#pragma once

#include <string>
#include <vector>
#include <assert.h>

namespace ecs
{
  using string = std::string;

  template <typename T>
  using vector = std::vector<T>;

} // namespace ecs

#define ECS_ASSERT assert