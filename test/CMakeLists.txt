cmake_minimum_required(VERSION 3.4.1)
project(Game_Boy_Emulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

enable_testing()

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})

# Define the source files.
set(SOURCE_FILES ../src/memory/memory
                 ../src/memory/cartridge
                 ../src/cpu/cpu
                 ../src/util/util)

# Define the location of the test ROMs.
set(TEST_ROM_FOLDER ${PROJECT_SOURCE_DIR}/test_roms)
set(INTRUCTIONS_TEST_ROM ${TEST_ROM_FOLDER}/instructions_test.gb)
set(TIMING_TEST_ROM ${TEST_ROM_FOLDER}/timing_test.gb)

# Build the test binaries.
add_executable(MemoryTest memory/memory_test ${SOURCE_FILES})
add_executable(CartridgeTest memory/cartridge_test ${SOURCE_FILES})
add_executable(CPUTest cpu/cpu_test ${SOURCE_FILES})

# Inject the location of the test ROMs into the tests.
add_definitions(-DINTRUCTIONS_TEST_ROM="${INTRUCTIONS_TEST_ROM}")
add_definitions(-DTIMING_TEST_ROM="${TIMING_TEST_ROM}")

# Link the test binaries with GMock and GTest libraries.
target_link_libraries(MemoryTest ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES})
target_link_libraries(CartridgeTest ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES})
target_link_libraries(CPUTest ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES})

set(EXECUTABLE_OUTPUT_PATH ../bin/tests)

# Add test binaries as runnable tests so that they can be run using ctest.
add_test(CartridgeTests ${EXECUTABLE_OUTPUT_PATH}/CartridgeTest)
add_test(MemoryTests ${EXECUTABLE_OUTPUT_PATH}/MemoryTest)
add_test(CPUTests ${EXECUTABLE_OUTPUT_PATH}/CPUTest)