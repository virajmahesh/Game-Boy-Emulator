cmake_minimum_required(VERSION 3.4.1)
project(Game_Boy_Emulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

enable_testing()

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)
find_package(OpenGL)
find_package(GLFW 3.0.0)

# Include the required directories
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIR})
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})

# Define the source files.
set(SOURCE_FILES ../src/memory/memory
                 ../src/memory/cartridge
                 ../src/cpu/cpu
                 ../src/util/util
                 ../src/gpu/gpu
                 ../src/input/keyboard)

# Define the location of the test ROMs.
set(TEST_ROM_FOLDER ${PROJECT_SOURCE_DIR}/integration/test_roms)
set(TIMER_TEST_ROM_FOLDER ${TEST_ROM_FOLDER}/timer)
set(CPU_TEST_ROM_FOLDER ${TEST_ROM_FOLDER}/cpu)
set(BOOT_TEST_ROM_FOLDER ${TEST_ROM_FOLDER}/boot)
set(MEMORY_TEST_ROM_FOLDER ${TEST_ROM_FOLDER}/memory)

add_definitions(-DTIMER_TEST_ROM_PATH="${TIMER_TEST_ROM_FOLDER}/%s")
add_definitions(-DCPU_TEST_ROM_PATH="${CPU_TEST_ROM_FOLDER}/%s")
add_definitions(-DBOOT_TEST_ROM_PATH="${BOOT_TEST_ROM_FOLDER}/%s")
add_definitions(-DMEMORY_TEST_ROM_PATH="${MEMORY_TEST_ROM_FOLDER}/%s")

# Build the test binaries.
add_executable(MemoryUnitTests memory/memory_unit_test ${SOURCE_FILES})
add_executable(CartridgeUnitTests memory/cartridge_unit_test ${SOURCE_FILES})
add_executable(CPUUnitTests cpu/cpu_unit_test ${SOURCE_FILES})
add_executable(IntegrationTests integration/integration_test ${SOURCE_FILES})

# Inject the location of the test ROMs into the tests.

# Link the test binaries with GMock and GTest libraries.
target_link_libraries(MemoryUnitTests ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
target_link_libraries(CartridgeUnitTests ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
target_link_libraries(CPUUnitTests ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
target_link_libraries(IntegrationTests ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})

set(EXECUTABLE_OUTPUT_PATH ../bin/tests)

# Add test binaries as runnable tests so that they can be run using ctest.
add_test(CartridgeUnitTests ${EXECUTABLE_OUTPUT_PATH}/CartridgeTests)
add_test(MemoryUnitTests ${EXECUTABLE_OUTPUT_PATH}/MemoryTests)
add_test(CPUUnitTests ${EXECUTABLE_OUTPUT_PATH}/CPUTests)
add_test(IntegrationTests ${EXECUTABLE_OUTPUT_PATH}/IntegrationTests)