#pragma once

using Print = void (*)(const void *);
struct UserFunctions
{
  const Print print;
};