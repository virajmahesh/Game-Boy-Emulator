# Game Boy Emulator
Emulator for the original Game Boy.

## Project Overview
All source files are in the `src/` directory.
Source files for tests are in the `test/` directory.

Currently, each module's tests are compiled into their own executable.

## Building the emulator
This project supports `cmake`. On systems that support `make`, such as OS X and Linux, you can build the emulator and all tests using the following commands:
```
cmake .
make
```
   
This will compile the executable to the `bin/` directory at the root level of the project.
Tests will be compile to the `bin/tests` directory.

## Dependencies
The emulator depends on `GLFW` and `OpenGL`. The `FindGLFW` cmake module locates the `GLFW` library and links to it at compile time.

## Miscellaneous
This emulator uses the DMGBoy emulator (https://github.com/rtfpessoa/dmgboy) as a reference emulator. 

