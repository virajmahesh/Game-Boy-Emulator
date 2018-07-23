/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 * Tests that the memory module correctly handles basic load and store operations.
 */

#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <ctype.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../test_util.h"
#include "../../src/memory/memory.h"

using namespace testing;

class MockCartridge: public Cartridge {
public:
    MOCK_METHOD1(load_byte_rom, uint8_t(uint16_t));
    MOCK_METHOD2(store_byte_rom, void(uint16_t, uint8_t));

    MOCK_METHOD1(load_word_rom, uint16_t(uint16_t));
    MOCK_METHOD2(store_word_rom, void(uint16_t, uint16_t));

    MOCK_METHOD1(load_byte_ram, uint8_t(uint16_t));
    MOCK_METHOD2(store_byte_ram, void(uint16_t, uint8_t));

    MOCK_METHOD1(load_word_ram, uint16_t(uint16_t));
    MOCK_METHOD2(store_word_ram, void(uint16_t, uint16_t));
};

/*
 * Test that the memory module correctly stores and loads a byte to and from
 * a given address.
 */
TEST(Memory_Test, Load_And_Store_Byte_RAM) {
    StrictMock<MockCartridge> mock_cartridge;
    Memory memory = Memory(mock_cartridge);

    uint8_t data = random_byte();
    uint16_t address = random_word(0xC000, 0xCFFF);

    memory.store_byte(address, data);

    // Test that the byte was stored correctly by reading it back.
    EXPECT_EQ(data, memory.load_byte(address));
}

/*
 * Test that the memory module correctly stores and loads a word to and from
 * a given address. Checks that words are stored with the least significant
 * byte first.
 */
TEST(Memory_Test, Load_And_Store_Word_RAM) {
    StrictMock<MockCartridge> mock_cartridge;
    Memory memory = Memory(mock_cartridge);

    uint16_t data = random_word();
    uint16_t address = random_word(0xC000, 0xCFFF);

    memory.store_word(address, data);

    // Test that the word was stored correctly by reading the entire word.
    EXPECT_EQ(data, memory.load_word(address));

    // Test that the word was stored correctly by loading both bytes seperately.
    EXPECT_EQ(low_byte(data), memory.load_byte(address));
    EXPECT_EQ(high_byte(data), memory.load_byte(address + 0x0001));
}

/*
 * Test that the address space is correctly partitioned and that calls to
 * load are correctly delegated.
 */
TEST(Memory_Test, Address_Space_Partitioning_Load_Ops) {
    StrictMock<MockCartridge> mock_cartridge;
    Memory memory = Memory(mock_cartridge);

    uint16_t rom_addr = random_word(0x0000, 0x8000);
    uint16_t ram_addr = random_word(0xA000, 0xBFFF);

    EXPECT_CALL(mock_cartridge, load_byte_rom(rom_addr))
            .Times(1)
            .WillOnce(Return(0xFF));

    EXPECT_CALL(mock_cartridge, load_byte_ram(ram_addr))
            .Times(1)
            .WillOnce(Return(0xEE));

    EXPECT_CALL(mock_cartridge, load_word_rom(rom_addr))
            .Times(1)
            .WillOnce(Return(0xFFEE));

    EXPECT_CALL(mock_cartridge, load_word_ram(ram_addr))
            .Times(1)
            .WillOnce(Return(0xEEFF));

    EXPECT_EQ(0xFF, memory.load_byte(rom_addr));
    EXPECT_EQ(0xEE, memory.load_byte(ram_addr));
    EXPECT_EQ(0xFFEE, memory.load_word(rom_addr));
    EXPECT_EQ(0xEEFF, memory.load_word(ram_addr));
}

/*
 * Test that the address space is correctly partitioned and that calls to
 * store are correctly delegated.
 */
TEST(Memory_Test, Address_Space_Partitioning_Store_Ops) {
    StrictMock<MockCartridge> mock_cartridge;
    Memory memory = Memory(mock_cartridge);

    uint16_t rom_addr = random_word(0x0000, 0x8000);
    uint16_t ram_addr = random_word(0xA000, 0xBFFF);

    EXPECT_CALL(mock_cartridge, store_byte_rom(rom_addr, 0x12)).Times(1);
    EXPECT_CALL(mock_cartridge, store_byte_ram(ram_addr, 0x34)).Times(1);

    EXPECT_CALL(mock_cartridge, store_word_rom(rom_addr, 0x1234)).Times(1);
    EXPECT_CALL(mock_cartridge, store_word_ram(ram_addr, 0x5678)).Times(1);

    memory.store_byte(rom_addr, 0x12);
    memory.store_byte(ram_addr, 0x34);
    memory.store_word(rom_addr, 0x1234);
    memory.store_word(ram_addr, 0x5678);
}

TEST(Memory_Test, Memory_Flags_Access) {
    StrictMock<MockCartridge> mock_cartridge;
    Memory memory = Memory(mock_cartridge);

    EXPECT_EQ(false, memory.get_flag(RESET_DIV_CYCLES_FLAG));
    EXPECT_EQ(false, memory.get_flag(RELOAD_TIMER_A_FLAG));

    memory.set_flag(RESET_DIV_CYCLES_FLAG, true);
    EXPECT_EQ(true, memory.get_flag(RESET_DIV_CYCLES_FLAG));
    EXPECT_EQ(false, memory.get_flag(RELOAD_TIMER_A_FLAG));

    EXPECT_EQ(true, memory.get_flag(RESET_DIV_CYCLES_FLAG));
    EXPECT_EQ(false, memory.get_flag(RELOAD_TIMER_A_FLAG));

    memory.set_flag(RELOAD_TIMER_A_FLAG, true);
    EXPECT_EQ(true, memory.get_flag(RESET_DIV_CYCLES_FLAG));
    EXPECT_EQ(true, memory.get_flag(RELOAD_TIMER_A_FLAG));

    memory.set_flag(RESET_DIV_CYCLES_FLAG, false);
    EXPECT_EQ(false, memory.get_flag(RESET_DIV_CYCLES_FLAG));
    EXPECT_EQ(true, memory.get_flag(RELOAD_TIMER_A_FLAG));
}

TEST(Memory_Test, Memory_Flags_Write_to_DIV) {
    StrictMock<MockCartridge> mock_cartridge;
    Memory memory = Memory(mock_cartridge);

    EXPECT_EQ(false, memory.get_flag(RESET_DIV_CYCLES_FLAG));

    memory.store_byte(DIV, 0x00);

    EXPECT_EQ(memory.get_flag(RESET_DIV_CYCLES_FLAG), true);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}