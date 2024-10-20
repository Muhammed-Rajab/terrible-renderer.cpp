add_executable(raymarching raymarching.cpp)

target_compile_options(raymarching PRIVATE -Wall -O3)

target_include_directories(raymarching PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_sources(raymarching PRIVATE ${CMAKE_SOURCE_DIR}/src/renderer.cpp)
