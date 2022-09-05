#pragma once
#include <time.h>
#include <stdio.h>
#include <ctime>

struct TimeProfile
{
  using ns_t = unsigned long long;

  const char *label;
  ns_t start;
  bool stoped;
  TimeProfile(const char *label) : label(label), stoped(false)
  {
    start = get_us();
  }
  ns_t get_time()
  {
    stoped = true;
    return get_us() - start;
  }
  ns_t stop()
  {
    ns_t dif = get_us() - start;
    printf("took %llu us. %s\n", dif, label);
    return dif;
  }
  ~TimeProfile()
  {
    if (!stoped)
      stop();
  }

private:
  static ns_t get_us()
  {
    std::timespec ts;
    std::timespec_get(&ts, TIME_UTC);
    return ts.tv_nsec / 1000 + ts.tv_sec * 1000'000u;
  }
};