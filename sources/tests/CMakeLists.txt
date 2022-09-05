
include_directories(include)

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)


add_executable(test0 ${ROOT}/test0.cpp)

target_link_libraries(test0 ecsLib ${ADDITIONAL_LIBS})

target_compile_definitions(ecsLib PUBLIC USER_FUNCTION_HEADER=<ecs/c.h>)