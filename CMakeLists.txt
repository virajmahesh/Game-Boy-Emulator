cmake_minimum_required(VERSION 3.4)
project(Game_Boy_Emulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main/main.cpp src/memory/memory.cpp src/memory/memory.h)
add_executable(Game_Boy_Emulator ${SOURCE_FILES})

set(EXECUTABLE_OUTPUT_PATH bin)