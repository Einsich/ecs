#pragma once

#include <string>
#include <vector>
#include <array>
#include <queue>
#include <any>
#include <algorithm>
#include <functional> //only for eventsQueue
#include <assert.h>

namespace ecs
{
  using string = std::string;

  template <typename T>
  using vector = std::vector<T>;
  template <typename T>
  using queue = std::queue<T>;
  template <typename T, std::size_t N>
  using array = std::array<T, N>;

  using any = std::any;

  using uint = unsigned int;
  using byte = unsigned char;

  template <class _RanIt, class _Pr>
  inline void sort(const _RanIt _First, const _RanIt _Last, _Pr _Pred)
  {
    std::sort(_First, _Last, _Pred);
  }

  template <typename T>
  struct is_zero_sizeof_t : T
  {
    int x;
  };
  template <typename T>
  inline constexpr bool is_zero_sizeof = sizeof(int) == sizeof(is_zero_sizeof_t<T>);


  using LogFunction = void (*)(const char *format, ...);
  //you can override this for custom logging
  extern LogFunction ecs_log;
  extern LogFunction ecs_error;

} // namespace ecs

#define ECS_ASSERT assert
#define ECS_ASSERT_RETURN(assertion, ret_value) \
  if (!(assertion))                             \
  {                                             \
    assert(assertion);                          \
    return ret_value;                           \
  }
#define ECS_LOG ecs::ecs_log
#define ECS_ERROR ecs::ecs_error

#define __CONCAT_HELPER__(x, y) x##y
#define __CONCAT__(x, y) __CONCAT_HELPER__(x, y)

//#define ECS_INLINE __forceinline
#define ECS_INLINE

#define ECS_OPTIMIZED_DESTRUCTION 0
#define ECS_CLEAR_MEM_PATTERN 0x12341234