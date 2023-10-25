cmake_minimum_required(VERSION 3.5.0)
project(FileSyncer VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_library(wrappers SHARED wrapper_functions.c wrapper_functions.h)
target_link_libraries(wrappers PRIVATE wsock32 ws2_32)

add_executable(server server.c)
target_link_libraries(server PRIVATE wrappers wsock32 ws2_32)

add_executable(client client.c)
target_link_libraries(client PRIVATE wrappers wsock32 ws2_32)

file(COPY ../configs/ DESTINATION ../build/)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
