cmake_minimum_required(VERSION 3.5.0)
project(FileSyncer VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_executable(server server.c)
add_executable(client client.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
