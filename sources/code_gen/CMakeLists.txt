cmake_minimum_required(VERSION 3.10)

set(ADDITIONAL_LIBS LIBCMT.lib)

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

project(ecs_code_gen)
add_executable(ecs_code_gen main.cpp)
target_link_libraries(ecs_code_gen ${ADDITIONAL_LIBS})
