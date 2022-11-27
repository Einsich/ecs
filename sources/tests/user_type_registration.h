#pragma once
#include <iostream>
#include "user_type_functions.h"

template <typename T>
void print(const void *raw_memory)
{
  std::cout << *(const T *)raw_memory;
}

template <typename T>
UserFunctions get_user_functions()
{
  return UserFunctions{print<T>()};
}