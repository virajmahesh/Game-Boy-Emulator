/*
 * @author: Viraj Mahesh
 */
#include <GLFW/glfw3.h>

#include "../../src/cpu/cpu.h"
#include "../../src/gpu/gpu.h"
#include "../../src/input/keyboard.h"

/*
 * Execute a ROM for the specified number of cycles.
 *
 * @param rom_file_name: The path to the ROM you want to execute.
 * @param max_instructions: The number of instructions to execute before terminating.
 * @return: The hash of the Game Boy screen.
 */
uint64_t execute_rom(const char * rom_file_name, uint32_t max_instructions) {
    ifstream rom_file = ifstream(rom_file_name);

    Cartridge cartridge = Cartridge(read_file(rom_file));
    Memory memory = Memory(cartridge);
    CPU cpu = CPU(memory);
    GPU gpu = GPU(memory);
    Keyboard keyboard = Keyboard(gpu, memory);

    uint32_t cycles = 0;

    while (cpu.get_num_instructions() < max_instructions) {
        cycles = cpu.execute_next_instr();
        cpu.handle_interrupts();
        gpu.render_screen(cycles);

        keyboard.process_key_events();
    }

    glfwTerminate();
    return gpu.screen_hash();
}

int main(int argc, char **argv) {
    char * rom_file_name = argv[1];
    uint32_t max_instructions = (uint32_t) atoi(argv[2]);

    cout << hex << execute_rom(rom_file_name, max_instructions);
}