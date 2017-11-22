cmake_minimum_required(VERSION 3.8)
project("Game of Life")

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES 501-game-of-life.c)
add_executable(example ${SOURCE_FILES})