cmake_minimum_required(VERSION 3.5.0)
project(FileSyncer VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_library(wrappers SHARED wrapper_functions.c wrapper_functions.h)
target_link_libraries(wrappers PRIVATE wsock32 ws2_32)

add_library(del SHARED directory_event_listener.c directory_event_listener.h)
target_link_libraries(del PRIVATE wrappers)

add_library(ds SHARED data_structures/hash_map/hash_map.c data_structures/hash_map/hash_map.h)
target_link_libraries(ds PRIVATE wrappers)

add_executable(server server.c)
target_link_libraries(server PRIVATE wrappers wsock32 ws2_32 del ds)

add_executable(client client.c)
target_link_libraries(client PRIVATE wrappers wsock32 ws2_32 del ds)

configure_file(../configs/server.conf ../build/server.conf COPYONLY)
configure_file(../configs/directorylinks.conf ../build/directorylinks.conf COPYONLY)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
