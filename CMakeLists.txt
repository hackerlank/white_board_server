cmake_minimum_required (VERSION 2.8)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3  -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 ")

project (game_server_linux)
if (NOT SET_OUTPUT_DIRECTORY)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/target")
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib")
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib")
    set (SET_OUTPUT_DIRECTORY ON)
endif (NOT SET_OUTPUT_DIRECTORY)

add_subdirectory ("${PROJECT_SOURCE_DIR}/framework" build/lib-framework)
add_subdirectory ("${PROJECT_SOURCE_DIR}/game_base" build/lib-game_base)


add_subdirectory ("${PROJECT_SOURCE_DIR}/DN_application/DN_Account" build/DN_application/DN_Account)
add_subdirectory ("${PROJECT_SOURCE_DIR}/runner" build/bin-runner)
