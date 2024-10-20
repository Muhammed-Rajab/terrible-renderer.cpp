add_executable(sphere sphere.cpp)

target_compile_options(sphere PRIVATE -Wall -O3)

target_include_directories(sphere PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_sources(sphere PRIVATE ${CMAKE_SOURCE_DIR}/src/renderer.cpp)
