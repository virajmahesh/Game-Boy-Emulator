/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 */


#include "keyboard.h"

void Keyboard::process_key_events(GLFWwindow *window, Memory &memory) {
    uint8_t joyp = memory.load_byte(P1);
    uint8_t interrupt_flag = memory.load_byte(IF);

    if (bit(joyp, 4) == 0) {
        // UP, DOWN, LEFT and RIGHT keys
        if (glfwGetKey(window, RIGHT) == GLFW_PRESS) {
            reset_bit(joyp, 0);
            interrupt_flag |= 0b00010000;
        }
        else {
            set_bit(joyp, 0);
        }

        if (glfwGetKey(window, LEFT) == GLFW_PRESS) {
            reset_bit(joyp, 1);
            interrupt_flag |= 0b00010000;
        }
        else {
            set_bit(joyp, 1);
        }

        if (glfwGetKey(window, UP) == GLFW_PRESS) {
            reset_bit(joyp, 2);
            interrupt_flag |= 0b00010000;
        }
        else {
            set_bit(joyp, 2);
        }

        if (glfwGetKey(window, DOWN) == GLFW_PRESS) {
            reset_bit(joyp, 3);
            interrupt_flag |= 0b00010000;
        }
        else {
            set_bit(joyp, 3);
        }

    }
    else if (bit(joyp, 5) == 0) {
        // SELECT, START, A and B keys
        if (glfwGetKey(window, A) == GLFW_PRESS) {
            reset_bit(joyp, 0);
            interrupt_flag |= 0b00010000;
        }
        else {
            set_bit(joyp, 0);
        }

        if (glfwGetKey(window, B) == GLFW_PRESS) {
            reset_bit(joyp, 1);
            interrupt_flag |= 0b00010000;
        }
        else {
            set_bit(joyp, 1);
        }

        if (glfwGetKey(window, SELECT) == GLFW_PRESS) {
            reset_bit(joyp, 2);
            interrupt_flag |= 0b00010000;
        }
        else {
            set_bit(joyp, 2);
        }

        if (glfwGetKey(window, START) == GLFW_PRESS) {
            reset_bit(joyp, 3);
            interrupt_flag |= 0b00010000;
        }
        else {
            set_bit(joyp, 3);
        }
    }

    memory.store_byte(P1, joyp);
    memory.store_byte(IF, interrupt_flag);
}

