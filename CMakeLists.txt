cmake_minimum_required(VERSION 3.4.1)
project(Game_Boy_Emulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

set(SOURCE_FILES src/main/main
                 src/memory/memory
                 src/memory/cartridge
                 src/cpu/cpu
                 src/gpu/gpu
                 src/util/util)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

# Build tests
add_subdirectory(test)

# Find OpenGL and GLFW
find_package(OpenGL)
find_package(GLFW 3.0.0)

# Include the required directories
include_directories(${OPENGL_INCLUDE_DIRS}  ${GLFW_INCLUDE_DIR})

add_executable(GameBoyEmulator ${SOURCE_FILES})

target_link_libraries(GameBoyEmulator ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})


set(EXECUTABLE_OUTPUT_PATH bin)