cmake_minimum_required(VERSION 3.4.1)
project(Game_Boy_Emulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

enable_testing()

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})

set(SOURCE_FILES ../src/memory/memory
                 ../src/memory/cartridge
                 ../src/cpu/cpu
                 ../src/util/util)

add_executable(MemoryTest memory/memory_test ${SOURCE_FILES})
add_executable(CartridgeTest memory/cartridge_test ${SOURCE_FILES})
add_executable(CPUTest cpu/cpu_test ${SOURCE_FILES})


add_test(CartridgeTests ../bin/tests/CartridgeTest)
add_test(MemoryTests ../bin/tests/MemoryTest)
add_test(CPUTests ../bin/tests/CPUTest)

target_link_libraries(MemoryTest ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES})
target_link_libraries(CartridgeTest ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES})
target_link_libraries(CPUTest ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES})

set(EXECUTABLE_OUTPUT_PATH ../bin/tests)