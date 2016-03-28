/*
 * @author: Viraj Mahesh
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/cpu/cpu.h"

using namespace testing;

#define TIMING_TEST_RESULT "instr_timing\n\n\nPassed\n"
#define INTRUCTIONS_TEST_RESULT "cpu_instrs\n\n01:ok  02:ok  03:ok  04:ok  05:ok  06:ok  07:ok  08:ok  09:ok  10:ok  11:ok  \n\nPassed all tests\n"

/*
 * Create an extension of the Memory class that stores all data written to the
 * console in a buffer. Console writes are triggered by writing the value 0x81
 * to the SC register. This writes the value stored in the SB register to the
 * console.
 */
class TestMemory : public Memory {
private:
    string result;

public:
    TestMemory(Cartridge & cartridge) : Memory(cartridge) {}

    /*
     * Checks if 0x81 is being written to the SC register. If it is, then the
     * character stored in the SB register is appended to the result buffer.
     * Otherwise, the byte is stored as expected.
     *
     * @param address: The memory address to write to.
     * @param value: The byte to be written.
     */
    void store_byte(uint16_t address, uint8_t value) {
        if (address == SC and value == 0x81) {
            result += load_byte(SB);
        }
        else {
            Memory::store_byte(address, value);
        }
    }

    /*
     * @return: The data stored in the buffer. Use this method to verify that
     * the test ran correctly.
     */
    string get_result() {
        return result;
    }
};

/*
 * Test that the CPU module correctly executes instructions_test.
 */
TEST(Integration_Test, Instructions_Test) {
    ifstream rom_file = ifstream(INTRUCTIONS_TEST_ROM);

    Cartridge cartridge = Cartridge(read_file(rom_file));
    TestMemory memory = TestMemory(cartridge);

    CPU cpu = CPU(memory);

    while (cpu.get_num_instructions() < 24750000) {
        cpu.execute_next_instr();
        cpu.handle_interrupts();
    }

    EXPECT_EQ(INTRUCTIONS_TEST_RESULT, memory.get_result());
}

/*
 * Test that the CPU module correctly executes timing_test.
 */
TEST(CPU_ROM_Test, Timing_Test) {
    ifstream rom_file = ifstream(TIMING_TEST_ROM);

    Cartridge cartridge = Cartridge(read_file(rom_file));
    TestMemory memory = TestMemory(cartridge);

    CPU cpu = CPU(memory);

    while (cpu.get_num_instructions() < 900000) {
        cpu.execute_next_instr();
        cpu.handle_interrupts();
    }

    EXPECT_EQ(TIMING_TEST_RESULT, memory.get_result());
}

int main(int argc, char **argv) {
    srand(time(NULL));
    InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}