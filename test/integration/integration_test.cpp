/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 */

#include <gtest/gtest.h>

#include "../../src/cpu/cpu.h"
#include "../../src/gpu/gpu.h"
#include "../../src/input/keyboard.h"

using namespace testing;

char file_path[1024];

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

    while (gpu.window_open() && cpu.get_num_instructions() < max_instructions) {
        cycles = cpu.execute_next_instr();
        cpu.handle_interrupts();
        gpu.render_screen(cycles);

        keyboard.process_key_events();
    }

    glfwTerminate();
    return gpu.screen_hash();
}

/*
 * Test that the emulator passes the DIV write test
 */
TEST(Timer_Test, DIV_Write_Test) {
    sprintf(file_path, TIMER_TEST_ROM_PATH, "div_write.gb");
    uint64_t screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x7dce967813f, screen_hash);
}

/*
 * Test that the emulator passes the TIMA increment tests
 */
TEST(Timer_Test, TIMA_Increment_Test) {
    sprintf(file_path, TIMER_TEST_ROM_PATH, "tima_increment_1.gb");
    uint64_t screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x7ad768a3131, screen_hash);

    sprintf(file_path, TIMER_TEST_ROM_PATH, "tima_increment_2.gb");
    screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x6af2d613bc4, screen_hash);

    sprintf(file_path, TIMER_TEST_ROM_PATH, "tima_increment_3.gb");
    screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x7ad768a3131, screen_hash);

    sprintf(file_path, TIMER_TEST_ROM_PATH, "tima_increment_4.gb");
    screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x7ad768a3131, screen_hash);
}

/*
 * Test that the emulator passes the DIV trigger tests
 */
TEST(Timer_Test, DIV_Trigger_Test) {
    sprintf(file_path, TIMER_TEST_ROM_PATH, "tima_div_trigger_1.gb");
    uint64_t screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x7ad768a3131, screen_hash);

    sprintf(file_path, TIMER_TEST_ROM_PATH, "tima_div_trigger_2.gb");
    screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x7601742e30c, screen_hash);

    sprintf(file_path, TIMER_TEST_ROM_PATH, "tima_div_trigger_3.gb");
    screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x63e83d1630f, screen_hash);

    sprintf(file_path, TIMER_TEST_ROM_PATH, "tima_div_trigger_4.gb");
    screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x7ad768a3131, screen_hash);
}

/*
 * Test that the TIMA register is correctly reloaded after overflow
 */
TEST(Timer_Test, TIMA_Reload_Test) {
    sprintf(file_path, TIMER_TEST_ROM_PATH, "tima_reload.gb");
    uint64_t screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x8d02c9a80ac, screen_hash);
}

/*
 * Test that writes to TIMA during reloading are correctly handled
 */
TEST(Timer_Test, TIMA_Write_Reload_Test) {
    sprintf(file_path, TIMER_TEST_ROM_PATH, "tima_write_reload.gb");
    uint64_t screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x6f5f240884e, screen_hash);
}

/*
 * Test that writes to TMA during reloading are correctly handled
 */
TEST(Timer_Test, TMA_Write_Reload_Test) {
    sprintf(file_path, TIMER_TEST_ROM_PATH, "tma_write_reload.gb");
    uint64_t screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x6750a7f4a48, screen_hash);
}

/*
 * Test that rapidly starting and stopping the timer works correctly
 */
TEST(Timer_Test, Rapid_Toggle_Test) {
    sprintf(file_path, TIMER_TEST_ROM_PATH, "rapid_toggle.gb");
    uint64_t screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x0, screen_hash);
}

/*
 * Test that the  emultor passes Blargg's CPU instruction tests
 */
TEST(CPU_Test, Blargg_Instruction_Test) {
    sprintf(file_path, CPU_TEST_ROM_PATH, "instructions_test.gb");
    uint64_t screen_hash = execute_rom(file_path, 25000000);
    ASSERT_EQ(0x717209ded53, screen_hash);
}

/*
 * Test that the bottom 4 bits of the F register always return 0
 */
TEST(CPU_Test, CPU_Reg_F) {
    sprintf(file_path, CPU_TEST_ROM_PATH, "cpu_reg_f.gb");
    uint64_t screen_hash = execute_rom(file_path, 500000);
    ASSERT_EQ(0x6cf2c27b2c6, screen_hash);
}

/*
 * Test that the CPU registers have the right value at boot
 */
TEST(Boot_Test, Boot_Registers) {
    sprintf(file_path, BOOT_TEST_ROM_PATH, "boot_regs-dmg0.gb");
    uint64_t screen_hash = execute_rom(file_path, 300000);
    ASSERT_EQ(0x8b5d0c766cb, screen_hash);
}

/*
 * Test that the hardware registers have the right value at boot
 */
TEST(Boot_Test, Boot_HWIO) {
    sprintf(file_path, BOOT_TEST_ROM_PATH, "boot_hwio-dmg0.gb");
    uint64_t screen_hash = execute_rom(file_path, 300000);
    ASSERT_EQ(0x0, screen_hash);
}

/*
 * Test that the OAM area has no unusable bits
 */
TEST(Memory_Test, OAM) {
    sprintf(file_path, MEMORY_TEST_ROM_PATH, "mem_oam.gb");
    uint64_t screen_hash = execute_rom(file_path, 300000);
    ASSERT_EQ(0x7dce967813f, screen_hash);
}

/*
 * Test behavior of unused HWIO bits
 */
TEST(Memory_Test, Unused_HWIO) {
    sprintf(file_path, MEMORY_TEST_ROM_PATH, "unused_hwio-GS.gb");
    uint64_t screen_hash = execute_rom(file_path, 300000);
    ASSERT_EQ(0x7dce967813f, screen_hash);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
