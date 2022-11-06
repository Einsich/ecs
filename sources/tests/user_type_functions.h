#pragma once
#include <iostream>

using Print = void (*)(const void *);
struct UserFunctions
{
  const Print print;
};
template <typename T>
void print(const void *raw_memory)
{
  std::cout << *(const T *)raw_memory;
}