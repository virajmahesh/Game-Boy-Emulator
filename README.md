# Game Boy Emulator
Emulator for the original Game Boy.

## Building the emulator
This project supports `cmake`. On systems that support `make`, such as OS X and Linux, you can build the emulator and all tests using the following commands:
```
cmake .
make
```

This will compile the executable to the `bin/` directory at the root level of the project.
Tests will be compiled to the `bin/tests` directory.

## Testing
Tests include unit tests as well as Blargg's test ROMs.

If your system supports ctest, you can use it to run the tests using the following commands:
```
cd test
ctest
```
Tests can also be run by executing individual test binaries.

The following games are currently known to work with the emulator:
* Tetris

## Dependencies
The emulator depends on the following libraries:
* `GLFW` and `OpenGL` for rendering the screen.
* `GTest` and `GMock` for testing and mocking.

