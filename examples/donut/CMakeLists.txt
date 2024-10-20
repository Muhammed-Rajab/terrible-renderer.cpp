add_executable(donut donut.cpp)

target_compile_options(donut PRIVATE -Wall -O3)

target_include_directories(donut PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_sources(donut PRIVATE ${CMAKE_SOURCE_DIR}/src/renderer.cpp)
