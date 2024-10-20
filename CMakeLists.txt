cmake_minimum_required(VERSION 3.10)

project(TerribleRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(examples/donut)
add_subdirectory(examples/raymarching)
add_subdirectory(examples/shader)
# add_subdirectory(examples/game)
# add_subdirectory(examples/sphere)