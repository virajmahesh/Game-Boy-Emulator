/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 */


#include "keyboard.h"

Keyboard::Keyboard(GPU &gpu, Memory &memory) : memory(memory) {
    window = gpu.get_window();
}

void Keyboard::process_key_events() {
    uint8_t joyp = memory.load_byte(P1);
    uint8_t interrupt_flag = memory.load_byte(IF);

    for (int i = 0; i < COLUMNS; i++) {
        if (get_bit(joyp, i + ROWS) == 0) {

            for (int j = 0; j < ROWS; j++) {

                // If the key has been pressed, reset the appropriate bit.
                if (glfwGetKey(window, keys[i][j]) == GLFW_PRESS) {
                    reset_bit(joyp, j);
                    set_bit(interrupt_flag, 4);
                }
                else {
                    set_bit(joyp, j);
                }
            }

        }
    }

    memory.store_byte(P1, joyp);
    memory.store_byte(IF, interrupt_flag);
}

