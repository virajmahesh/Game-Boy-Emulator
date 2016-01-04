# Game Boy Emulator
Emulator for the original Game Boy and Game Boy Color systems.

## Project Overview
All source files are in the `src/` directory.
Source files for tests are in the `test/` directory.

Currently, each module's tests are compiled into their own executable.

## Building the emulator
This project support `cmake`. On systems that support `make` such as OS X and Linux, you can build the emulator and all tests using the following commands:
```
cmake CMakeLists.txt
make
```
   
This will compile the executable to the `bin/` directory at the root level of the project.
Tests will be compile to the `bin/tests` directory.

## Miscellaneous
This emulator uses the DMGBoy emulator (https://github.com/rtfpessoa/dmgboy) as a reference emulator. 

