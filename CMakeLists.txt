cmake_minimum_required(VERSION 3.10)


set(SRC_ROOT ${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-m64 -Wall -Wextra -Wno-pragma-pack -Wno-deprecated-declarations -g")



if(BUILD_TYPE STREQUAL "dbg")
  set(CMAKE_BUILD_TYPE "Debug")
endif()
if(BUILD_TYPE STREQUAL "dev")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()
if(BUILD_TYPE STREQUAL "rel")
  set(CMAKE_BUILD_TYPE "Release")
endif()


project(ecs)

add_subdirectory(${SRC_ROOT}/sources/ecs)
add_subdirectory(${SRC_ROOT}/sources/tests)
