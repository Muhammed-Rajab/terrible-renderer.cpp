cmake_minimum_required(VERSION 3.10)

project(TerribleRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(examples/donut)
add_subdirectory(examples/raymarching)
add_subdirectory(examples/shader)
add_subdirectory(examples/sphere)
add_subdirectory(examples/game)

add_executable(app main.cpp)

target_compile_options(app PRIVATE -Wall -O3)

target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_sources(app PRIVATE ${CMAKE_SOURCE_DIR}/src/renderer.cpp)

