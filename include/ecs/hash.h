#pragma once

namespace ecs
{
  constexpr unsigned int fnv_hash(const char *str)
  {
    unsigned int h = 2166136261u;
    const unsigned char *p = (const unsigned char *)str;
    for (; *p; ++p)
      h = (h * 16777619u) ^ *p;
    return h;
  }

  constexpr unsigned int hash(const char *str)
  {
    return fnv_hash(str);
  }
}