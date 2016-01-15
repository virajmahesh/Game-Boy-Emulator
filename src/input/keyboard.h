/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#ifndef GAME_BOY_EMULATOR_KEYBOARD_H
#define GAME_BOY_EMULATOR_KEYBOARD_H

#include <GLFW/glfw3.h>

#include "../util/util.h"
#include "../memory/memory.h"
#include "../gpu/gpu.h"

#define ROWS 4
#define COLUMNS 2

/*
 * Provides a method for reading input from the keyboard.
 */
class Keyboard {

private:
    // Key mappings.
    const int UP = GLFW_KEY_UP;
    const int DOWN = GLFW_KEY_DOWN;
    const int LEFT = GLFW_KEY_LEFT;
    const int RIGHT = GLFW_KEY_RIGHT;
    const int START = GLFW_KEY_ENTER;
    const int SELECT = GLFW_KEY_SPACE;
    const int A = GLFW_KEY_Z;
    const int B = GLFW_KEY_X;

    const int keys[COLUMNS][ROWS]= {{RIGHT, LEFT, UP, DOWN},
                                    {A, B, SELECT, START}};

    Memory & memory;

    GLFWwindow *window;

public:

    Keyboard(GPU & gpu, Memory & memory);

    /*
     * Check if any of the mapped keys are being pressed and set the
     * appropriate bits of the P1 register to 0. Also triggers a joypad
     * interrupt by setting the 4th bit of the interrupt flag to 1 if any of
     * the mapped keys are being pressed.
     */
    void process_key_events();
};

#endif
