cmake_minimum_required(VERSION 3.26)
project(224)

set(CMAKE_CXX_STANDARD 23)

add_executable(224 game.cpp)

target_link_libraries(${PROJECT_NAME} -lSDL2 -lSDL2_gfx -lSDL2_image -lSDL2_ttf)
