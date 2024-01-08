cmake_minimum_required(VERSION 3.15)

set(CRC_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)

project(Simulator)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib/imgui)
add_subdirectory(test)
add_subdirectory(src)
