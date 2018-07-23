#include <fstream>
#include <GLFW/glfw3.h>

#include "../cpu/cpu.h"
#include "../gpu/gpu.h"
#include "../input/keyboard.h"

using namespace std;

int main(int argc, char **argv) {
    // Parse arguments
    ifstream rom_file = ifstream(argv[1]);

    Cartridge cartridge = Cartridge(read_file(rom_file));
    Memory memory = Memory(cartridge);
    CPU cpu = CPU(memory);
    GPU gpu = GPU(memory);
    Keyboard keyboard = Keyboard(gpu, memory);

    // The number of cycles executed by the CPU.
    uint32_t cycles = 0;

    // Emulator continues to run until the window is closed
    while (gpu.window_open()) {
        cycles = cpu.execute_next_instr();
        cpu.handle_interrupts();
        gpu.render_screen(cycles);

        keyboard.process_key_events();
    }

    cout << cpu.get_num_instructions() << endl;
    cout << hex << gpu.screen_hash() << endl;

    glfwTerminate();
    return 0;
}