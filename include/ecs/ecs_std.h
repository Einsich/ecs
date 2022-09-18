#pragma once

#include <string>
#include <vector>
#include <any>
#include <algorithm>
#include <assert.h>

namespace ecs
{
  using string = std::string;

  template <typename T>
  using vector = std::vector<T>;

  using any = std::any;

  using uint = unsigned int;
  using byte = unsigned char;

  template <class _RanIt, class _Pr>
  inline void sort(const _RanIt _First, const _RanIt _Last, _Pr _Pred)
  {
    std::sort(_First, _Last, _Pred);
  }
} // namespace ecs

#define ECS_ASSERT assert

#define __CONCAT_HELPER__(x, y) x##y
#define __CONCAT__(x, y) __CONCAT_HELPER__(x, y)

//#define ECS_INLINE __forceinline
#define ECS_INLINE

#define ECS_OPTIMIZED_DESTRUCTION 0
#define ECS_CLEAR_MEM_PATTERN 0x12341234