/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#ifndef GAME_BOY_EMULATOR_KEYBOARD_H
#define GAME_BOY_EMULATOR_KEYBOARD_H

#include <GLFW/glfw3.h>

#include "../util/util.h"
#include "../memory/memory.h"

class Keyboard {
    // Key mappings.
    const static int UP = GLFW_KEY_UP;
    const static int DOWN = GLFW_KEY_DOWN;
    const static int LEFT = GLFW_KEY_LEFT;
    const static int RIGHT = GLFW_KEY_RIGHT;
    const static int START = GLFW_KEY_ENTER;
    const static int SELECT = GLFW_KEY_SPACE;
    const static int A = GLFW_KEY_Z;
    const static int B = GLFW_KEY_X;

public:

    /*
     * Checks whether any of the input keys have been pressed and sets the
     * appropriate bits on register P1.
     *
     * @param window: The window that received the key press.
     * @param memory: The Game Boy's memory module. This will be used to set
     * the value of register P1.
     */
    static void process_key_events(GLFWwindow * window, Memory & memory);
};

#endif
