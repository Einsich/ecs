

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/sources/tests)


add_executable(test0
  test0/main.inl.cpp
  common.cpp
)

add_executable(test1
  test1/main.cpp
  test1/await_contruction_test.inl.cpp
  common.cpp
)
add_executable(test2
  test2/main.cpp
  test2/deffered_creation.inl.cpp
  common.cpp
)
add_executable(test3
  test3/main.inl.cpp
)
add_executable(test4
  test4/main.inl.cpp
)
add_executable(test5
  test5/main.inl.cpp
)
add_executable(test6
  test6/main.inl.cpp
)
add_executable(test7
  test7/dijkstra.inl.cpp
  test7/dijkstra.cpp
  test7/main.inl.cpp
)
target_link_libraries(test0 ecsLib ${ADDITIONAL_LIBS})
target_link_libraries(test1 ecsLib ${ADDITIONAL_LIBS})
target_link_libraries(test2 ecsLib ${ADDITIONAL_LIBS})
target_link_libraries(test3 ecsLib ${ADDITIONAL_LIBS})
target_link_libraries(test4 ecsLib ${ADDITIONAL_LIBS})
target_link_libraries(test5 ecsLib ${ADDITIONAL_LIBS})
target_link_libraries(test6 ecsLib ${ADDITIONAL_LIBS})
target_link_libraries(test7 ecsLib ${ADDITIONAL_LIBS})

target_include_directories(ecsLib PUBLIC ${CMAKE_SOURCE_DIR}/sources/tests)
target_compile_definitions(ecsLib PUBLIC USER_FUNCTION_HEADER=<user_type_functions.h>)
target_compile_definitions(ecsLib PUBLIC USER_FUNCTION_REGISTRATION_HEADER=<user_type_registration.h>)