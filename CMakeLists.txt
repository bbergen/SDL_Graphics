cmake_minimum_required(VERSION 3.2)
project(SDL_Graphics)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(BALL_SOURCE ball.c)
set(PONG_SOURCE pong.c)
add_executable(SDL_Ball ${BALL_SOURCE})
add_executable(SDL_Pong ${PONG_SOURCE})

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

include_directories(SDL_Graphics ${SDL2_INCLUDE_DIR})
target_link_libraries(SDL_Ball ${SDL2_LIBRARIES})
target_link_libraries(SDL_Pong ${SDL2_LIBRARIES})