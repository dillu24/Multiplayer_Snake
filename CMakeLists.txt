cmake_minimum_required(VERSION 3.6)
project(systems)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.c server.c game.c game.h connection.c connection.h client.c)
add_executable(systems ${SOURCE_FILES})