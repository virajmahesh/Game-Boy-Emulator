cmake_minimum_required(VERSION 3.4)
project(Game_Boy_Emulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES ../src/memory/ram
                 ../src/memory/memory)

add_executable(MemoryTest memory/memory_test ${SOURCE_FILES})

target_link_libraries(MemoryTest /usr/local/lib/libgtest.a)

set(EXECUTABLE_OUTPUT_PATH ../bin/tests)