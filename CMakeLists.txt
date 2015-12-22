cmake_minimum_required(VERSION 3.4)
project(Game_Boy_Emulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main/main
                 src/memory/ram
                 src/memory/memory)

add_executable(GameBoyEmulator ${SOURCE_FILES})
add_subdirectory(test)

set(EXECUTABLE_OUTPUT_PATH bin)