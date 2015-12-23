/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 * Tests that the ROM module handles load, store and bank switching
 * operations correctly.
 */

#include <gtest/gtest.h>
#include "../test_util.h"
#include "../../src/memory/rom.h"

inline uint8_t rom_bank(uint8_t i, uint8_t j) {
    return ((j & 0b00000011) <<  5) | (((i == 0)? 1 : i) & 0b00011111);
}

inline uint32_t rom_addr(uint8_t i, uint8_t j, uint16_t addr) {
    return ROM::BANK_SIZE * rom_bank(i, j) + addr - 0x4000;
}

/*
 * Test that Bank 0 is accessible in cartridges with an MBC 1
 * controller in 16/8 mode.
 */
TEST(MBC1_16_8_ROM_Test, Load_Byte_From_ROM_Bank_0) {
    uint16_t addr = random_word(0x0000, 0x4000);
    ROM rom = ROM(MBC1_16_8, random_byte_array(ROM::BANK_SIZE));

    EXPECT_EQ(rom.access_rom_data(addr), rom.load_byte_rom(addr));
}

/*
 * Test that a cartridge with an MBC 1 controller in 16/8 mode can switch all
 * banks into the 0x4000 - 0x7FFF space. Also tests that the 0x00, 0x20, 0x40
 * and 0x60 banks are inaccessible.
 */
TEST(MBC1_16_8_ROM_TEST, Bank_Switching) {
    // Create a ROM with 128 ROM banks
    uint32_t rom_size = ROM::BANK_SIZE * 128;
    ROM rom = ROM(MBC1_16_8, random_byte_array(rom_size));

    for (uint8_t j = 0x00; j <= 0x03; j++) {
        for (uint8_t i = 0x01; i <= 0x1F; i++) {
            // Switch to another bank
            rom.store_byte_rom(random_word(0x2000, 0x4000), i);
            rom.store_byte_rom(random_word(0x4000, 0x6000), j);

            uint16_t addr = random_word(0x4000, 0x8000);
            uint8_t data = rom.access_rom_data(rom_addr(i, j, addr));

            EXPECT_EQ(data, rom.load_byte_rom(addr));
            EXPECT_EQ(rom_bank(i, j), rom.get_rom_bank());

            EXPECT_NE(0x00, rom.get_rom_bank());
            EXPECT_NE(0x20, rom.get_rom_bank());
            EXPECT_NE(0x40, rom.get_rom_bank());
            EXPECT_NE(0x60, rom.get_rom_bank());
        }
    }
}

/*
 * Test that a cartride with an MBC 1 controller in 4/32 mode can switch all
 * banks into the 0x4000 - 0x7FFF space. Also tests that the 0x00 bank is
 * inaccessible.
 */
TEST(MBC1_4_32_ROM_TEST, Bank_Switching) {
    // Create a ROM with the maximum number of RAM banks
    uint32_t rom_size = ROM::BANK_SIZE * 32;
    ROM rom = ROM(MBC1_4_32, random_byte_array(rom_size));

    for (uint8_t i = 0x00; i <= 0x01F; i++) {
        // Switch to another bank
        rom.store_byte_rom(random_word(0x2000, 0x4000), i);

        uint16_t addr = random_word(0x4000, 0x8000);
        uint8_t data = rom.access_rom_data(rom_addr(i, 0, addr));

        EXPECT_EQ(data, rom.load_byte_rom(addr));
        EXPECT_EQ(rom_bank(i, 0), rom.get_rom_bank());

        EXPECT_NE(0x00, rom.get_rom_bank());
    }
}