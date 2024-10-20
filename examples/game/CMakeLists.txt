add_executable(game game.cpp)

target_compile_options(game PRIVATE -Wall -O3)

target_include_directories(game PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(game PRIVATE ./include/)

target_sources(game PRIVATE ${CMAKE_SOURCE_DIR}/src/renderer.cpp)
