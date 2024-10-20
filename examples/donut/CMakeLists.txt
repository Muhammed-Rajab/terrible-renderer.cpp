add_executable(donut donut.cpp)

target_include_directories(donut PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_sources(donut PRIVATE ${CMAKE_SOURCE_DIR}/src/renderer.cpp)
