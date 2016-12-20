/*
 * @author: Viraj Mahesh
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <GLFW/glfw3.h>

#include "../../src/cpu/cpu.h"
#include "../../src/gpu/gpu.h"
#include "../../src/input/keyboard.h"

using namespace testing;

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

/*
 * Test that the Emulator correctly executes instructions_test.
 */
TEST(CPU_Test, Blargg_CPU_Test) {
    uint64_t result = execute_rom(BLARGG_CPU_INSTR_TEST_ROM, 25000000);

    // Verify that the test passed.
    if (result != 7795912863059) {
        FAIL();
    }
}

/*
 * Test that the Emulator correctly executes Blargg's instruction
 * timing test rom.
 */
TEST(Timing_Test, Blargg_Timing_Test) {
    uint64_t result = execute_rom(BLARGG_TIMING_TEST_ROM, 900000);

    // Verify that the test passed.
    if (result != 8825397585559) {
        FAIL();
    }
}

/*
 * Test that the Emulator correctly executes timing_test.
 */
TEST(Timing_Test, DIV_Write_Test) {
    uint64_t result = execute_rom(DIV_WRITE_TEST_ROM, 700000);

    // Verify that the test passed.
    if (result != 8645390074175) {
        FAIL();
    }
}

TEST(Timing_Test, TIMA_Increment_Test) {
    uint64_t result = execute_rom(TIMA_INCREMENT_TEST_ROM, 700000);

    // Verify that the test passed.
    if (result != 8441599504689) {
        FAIL();
    }
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */

int main(int argc, char **argv) {
    InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}