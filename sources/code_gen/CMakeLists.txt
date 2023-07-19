cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-m64 -Wall -Wextra -Wno-pragma-pack -Wno-deprecated-declarations -g")


project(ecs_code_gen)
add_executable(ecs_code_gen main.cpp)
target_link_libraries(ecs_code_gen)
