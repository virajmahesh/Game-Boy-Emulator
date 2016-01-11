cmake_minimum_required(VERSION 3.4.1)
project(Game_Boy_Emulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES ../src/memory/memory
                 ../src/memory/cartridge
                 ../src/cpu/cpu)

add_executable(MemoryTest memory/memory_test ${SOURCE_FILES})
add_executable(CartridgeTest memory/cartridge_test ${SOURCE_FILES})
add_executable(CPUTest cpu/cpu_test ${SOURCE_FILES})

target_link_libraries(MemoryTest /usr/local/lib/libgtest.a)
target_link_libraries(MemoryTest /usr/local/lib/libgmock.a)

target_link_libraries(CartridgeTest /usr/local/lib/libgtest.a)

target_link_libraries(CPUTest /usr/local/lib/libgtest.a)
target_link_libraries(CPUTest /usr/local/lib/libgmock.a)

set(EXECUTABLE_OUTPUT_PATH ../bin/tests)