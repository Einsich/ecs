#pragma once

#include <string>
#include <vector>
#include <any>
#include <assert.h>

namespace ecs
{
  using string = std::string;

  template <typename T>
  using vector = std::vector<T>;

  using any = std::any;

} // namespace ecs

#define ECS_ASSERT assert

#define __CONCAT_HELPER__(x, y) x##y
#define __CONCAT__(x, y) __CONCAT_HELPER__(x, y)