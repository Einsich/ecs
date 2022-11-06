set(LIB_NAME ecsLib)
set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(LIB_SOURCES )

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/sources/tests)

file(GLOB_RECURSE LIB_SOURCES ${LIB_PATH}/*.cpp)

add_library(${LIB_NAME} OBJECT ${LIB_SOURCES})