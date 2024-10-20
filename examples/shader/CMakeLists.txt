add_executable(shader shader.cpp)

target_compile_options(shader PRIVATE -Wall -O3)

target_include_directories(shader PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_sources(shader PRIVATE ${CMAKE_SOURCE_DIR}/src/renderer.cpp)
