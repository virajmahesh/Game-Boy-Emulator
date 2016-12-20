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
set(TIMING_TEST_ROM_FOLDER ${TEST_ROM_FOLDER}/timing)
set(CPU_TEST_ROM_FOLDER ${TEST_ROM_FOLDER}/cpu)

#################
# CPU Test ROMS #
#################
set(BLARGG_CPU_INSTR_TEST_ROM ${CPU_TEST_ROM_FOLDER}/instructions_test.gb)
add_definitions(-DBLARGG_CPU_INSTR_TEST_ROM="${BLARGG_CPU_INSTR_TEST_ROM}")

####################
# Timing Test ROMS #
####################
set(BLARGG_TIMING_TEST_ROM ${TIMING_TEST_ROM_FOLDER}/timing_test.gb)
add_definitions(-DBLARGG_TIMING_TEST_ROM="${BLARGG_TIMING_TEST_ROM}")

set(DIV_WRITE_TEST_ROM ${TIMING_TEST_ROM_FOLDER}/div_write.gb)
add_definitions(-DDIV_WRITE_TEST_ROM="${DIV_WRITE_TEST_ROM}")

set(TIMA_INCREMENT_1_TEST_ROM ${TIMING_TEST_ROM_FOLDER}/tima_increment_1.gb)
add_definitions(-DTIMA_INCREMENT_1_TEST_ROM="${TIMA_INCREMENT_1_TEST_ROM}")

set(TIMA_INCREMENT_2_TEST_ROM ${TIMING_TEST_ROM_FOLDER}/tima_increment_2.gb)
add_definitions(-DTIMA_INCREMENT_2_TEST_ROM="${TIMA_INCREMENT_2_TEST_ROM}")

set(TIMA_INCREMENT_3_TEST_ROM ${TIMING_TEST_ROM_FOLDER}/tima_increment_3.gb)
add_definitions(-DTIMA_INCREMENT_3_TEST_ROM="${TIMA_INCREMENT_3_TEST_ROM}")

set(TIMA_INCREMENT_4_TEST_ROM ${TIMING_TEST_ROM_FOLDER}/tima_increment_4.gb)
add_definitions(-DTIMA_INCREMENT_4_TEST_ROM="${TIMA_INCREMENT_4_TEST_ROM}")


# Build the test binaries.
add_executable(MemoryTests memory/memory_test ${SOURCE_FILES})
add_executable(CartridgeTests memory/cartridge_test ${SOURCE_FILES})
add_executable(CPUTests cpu/cpu_test ${SOURCE_FILES})
add_executable(IntegrationTests integration/integration_test ${SOURCE_FILES})

# Inject the location of the test ROMs into the tests.

# Link the test binaries with GMock and GTest libraries.
target_link_libraries(MemoryTests ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
target_link_libraries(CartridgeTests ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
target_link_libraries(CPUTests ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
target_link_libraries(IntegrationTests ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})

set(EXECUTABLE_OUTPUT_PATH ../bin/tests)

# Add test binaries as runnable tests so that they can be run using ctest.
add_test(CartridgeTests ${EXECUTABLE_OUTPUT_PATH}/CartridgeTests)
add_test(MemoryTests ${EXECUTABLE_OUTPUT_PATH}/MemoryTests)
add_test(CPUTests ${EXECUTABLE_OUTPUT_PATH}/CPUTests)
add_test(IntegrationTests ${EXECUTABLE_OUTPUT_PATH}/IntegrationTests)