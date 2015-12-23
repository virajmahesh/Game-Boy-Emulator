cmake_minimum_required(VERSION 3.4)
project(Game_Boy_Emulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES ../src/memory/memory
                 ../src/memory/rom)

add_executable(MemoryTest memory/memory_test ${SOURCE_FILES})
add_executable(ROMTest memory/rom_test ${SOURCE_FILES})

target_link_libraries(MemoryTest /usr/local/lib/libgtest.a)
target_link_libraries(MemoryTest /usr/local/lib/libgtest_main.a)

target_link_libraries(ROMTest /usr/local/lib/libgtest.a)
target_link_libraries(ROMTest /usr/local/lib/libgtest_main.a)

set(EXECUTABLE_OUTPUT_PATH ../bin/tests)