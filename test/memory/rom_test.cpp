/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 * Tests that the ROM module handles load, store and bank switching
 * operations correctly.
 */

#include <gtest/gtest.h>
#include "../test_util.h"
#include "../../src/memory/cartridge.h"

inline uint16_t rom_bank_mbc_1(uint8_t i, uint8_t j) {
    return ((j & 0x03) <<  5) | (((i == 0)? 1 : i) & 0x1F);
}

inline uint16_t rom_bank_mbc_2(uint8_t i) {
    return ((i == 0)? 1 : i) & 0x0F;
}

inline uint16_t rom_bank_mbc_3(uint8_t i) {
    return ((i == 0)? 1 : i) & 0x7F;
}

inline uint16_t rom_bank_mbc_5(uint16_t i, uint16_t j) {
    return ((j << 8) & 0x100) | i;
}

inline uint32_t rom_addr(uint16_t bank, uint16_t addr) {
    return Cartridge::BANK_SIZE * bank + addr - 0x4000;
}

/*
 * Test that Bank 0 is accessible in cartridges with an MBC 1
 * controller in 16/8 mode.
 */
TEST(MBC1_16_8_ROM_Test, Load_Byte_From_ROM_Bank_0) {
    uint32_t rom_size = Cartridge::BANK_SIZE;
    Cartridge rom = Cartridge(MBC1_16_8, random_byte_array(rom_size), rom_size, 0);

    uint16_t addr = random_word(0x0000, 0x4000);

    EXPECT_EQ(rom.access_rom_data(addr), rom.load_byte_rom(addr));
}

/*
 * Test that Bank 0 is accessible in cartridges with an MBC 1
 * controller in 4/32 mode.
 */
TEST(MBC1_4_32_ROM_Test, Load_Byte_From_ROM_Bank_0) {
    uint32_t rom_size = Cartridge::BANK_SIZE;
    Cartridge rom = Cartridge(MBC1_4_32, random_byte_array(rom_size), rom_size, 0);

    uint16_t addr = random_word(0x0000, 0x4000);

    EXPECT_EQ(rom.access_rom_data(addr), rom.load_byte_rom(addr));
}

/*
 * Test that Bank 0 is accessible in cartridges with an MBC 2 controller.
 */
TEST(MBC2_ROM_Test, Load_Byte_From_ROM_Bank_0) {
    uint32_t rom_size = Cartridge::BANK_SIZE;
    Cartridge rom = Cartridge(MBC2, random_byte_array(rom_size), rom_size, 0);

    uint16_t addr = random_word(0x0000, 0x4000);

    EXPECT_EQ(rom.access_rom_data(addr), rom.load_byte_rom(addr));
}

/*
 * Test that Bank 0 is accessible in cartridges with an MBC 3 controller.
 */
TEST(MBC3_ROM_Test, Load_Byte_From_ROM_Bank_0) {
    uint32_t rom_size = Cartridge::BANK_SIZE;
    Cartridge rom = Cartridge(MBC3, random_byte_array(rom_size), rom_size, 0);

    uint16_t addr = random_word(0x0000, 0x4000);

    EXPECT_EQ(rom.access_rom_data(addr), rom.load_byte_rom(addr));
}

/*
 * Test that Bank 0 is accessible in cartridges with an MBC 5 controller.
 */
TEST(MBC5_ROM_Test, Load_Byte_From_ROM_Bank_0) {
    uint32_t rom_size = Cartridge::BANK_SIZE;
    Cartridge rom = Cartridge(MBC5, random_byte_array(rom_size), rom_size, 0);

    uint16_t addr = random_word(0x0000, 0x4000);

    EXPECT_EQ(rom.access_rom_data(addr), rom.load_byte_rom(addr));
}

/*
 * Test that a cartridge with an MBC 1 controller in 16/8 mode can switch all
 * banks into the 0x4000 - 0x7FFF space. Also tests that the 0x00, 0x20, 0x40
 * and 0x60 banks are inaccessible.
 */
TEST(MBC1_16_8_ROM_TEST, Bank_Switching) {
    uint32_t rom_size = Cartridge::BANK_SIZE * 128;
    Cartridge rom = Cartridge(MBC1_16_8, random_byte_array(rom_size), rom_size, 0);

    for (uint8_t j = 0x00; j <= 0x03; j++) {
        for (uint8_t i = 0x01; i <= 0x1F; i++) {
            // Switch to another bank
            rom.store_byte_rom(random_word(0x2000, 0x4000), i);
            rom.store_byte_rom(random_word(0x4000, 0x6000), j);

            uint16_t bank = rom_bank_mbc_1(i, j);
            uint16_t addr = random_word(0x4000, 0x8000);
            uint8_t data = rom.access_rom_data(rom_addr(bank, addr));

            EXPECT_EQ(data, rom.load_byte_rom(addr));
            EXPECT_EQ(bank, rom.get_rom_bank());

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
    uint32_t rom_size = Cartridge::BANK_SIZE * 32;
    Cartridge rom = Cartridge(MBC1_4_32, random_byte_array(rom_size), rom_size, 0);

    for (uint8_t i = 0x00; i <= 0x01F; i++) {
        // Switch to another bank
        rom.store_byte_rom(random_word(0x2000, 0x4000), i);

        uint16_t bank = rom_bank_mbc_1(i, 0);
        uint16_t addr = random_word(0x4000, 0x8000);
        uint8_t data = rom.access_rom_data(rom_addr(bank, addr));

        EXPECT_EQ(data, rom.load_byte_rom(addr));
        EXPECT_EQ(bank, rom.get_rom_bank());

        EXPECT_NE(0x00, rom.get_rom_bank());
    }
}

/*
 * Test that a cartride with an MBC 2 controller can switch all banks into the
 * 0x4000 - 0x7FFF space. Also tests that the 0x00 bank is inaccessible.
 */
TEST(MBC2_ROM_TEST, Bank_Switching) {
    uint32_t rom_size = Cartridge::BANK_SIZE * 16;
    Cartridge rom = Cartridge(MBC2, random_byte_array(rom_size), rom_size, 0);

    for (uint8_t i = 0x00; i <= 0x0F; i++) {
        // Switch to another bank
        rom.store_byte_rom(random_word(0x2000, 0x4000), i);

        uint16_t bank = rom_bank_mbc_2(i);
        uint16_t addr = random_word(0x4000, 0x8000);
        uint8_t data = rom.access_rom_data(rom_addr(bank, addr));

        EXPECT_EQ(data, rom.load_byte_rom(addr));
        EXPECT_EQ(bank, rom.get_rom_bank());

        EXPECT_NE(0x00, rom.get_rom_bank());
    }
}

/*
 * Test that a cartride with an MBC 3 controller can switch all banks into the
 * 0x4000 - 0x7FFF space. Also tests that the 0x00 bank is inaccessible.
 */
TEST(MBC3_ROM_TEST, Bank_Switching) {
    uint32_t rom_size = Cartridge::BANK_SIZE * 128;
    Cartridge rom = Cartridge(MBC3, random_byte_array(rom_size), rom_size, 0);

    for (uint8_t i = 0x00; i <= 0x7F; i++) {
        // Switch to another bank
        rom.store_byte_rom(random_word(0x2000, 0x4000), i);

        uint16_t bank = rom_bank_mbc_3(i);
        uint16_t addr = random_word(0x4000, 0x8000);
        uint8_t data = rom.access_rom_data(rom_addr(bank, addr));

        EXPECT_EQ(data, rom.load_byte_rom(addr));
        EXPECT_EQ(bank, rom.get_rom_bank());

        EXPECT_NE(0x00, rom.get_rom_bank());
    }
}

/*
 * Test that a cartride with an MBC 5 controller can switch all banks into the
 * 0x4000 - 0x7FFF space. Also tests that the 0x00 bank is inaccessible.
 */
TEST(MBC5_ROM_TEST, Bank_Switching) {
    uint32_t rom_size = Cartridge::BANK_SIZE * 512;
    Cartridge rom = Cartridge(MBC5, random_byte_array(rom_size), rom_size, 0);

    for (uint16_t j = 0x00; j <= 0x01; j++) {
        for (uint16_t i = 0x00; i <= 0xFF; i++) {
            // Switch to another bank
            rom.store_byte_rom(random_word(0x2000, 0x2FFF), i);
            rom.store_byte_rom(random_word(0x3000, 0x3FFF), j);

            uint16_t bank = rom_bank_mbc_5(i, j);
            uint16_t addr = random_word(0x4000, 0x8000);
            uint8_t data = rom.access_rom_data(rom_addr(bank, addr));

            EXPECT_EQ(data, rom.load_byte_rom(addr));
            EXPECT_EQ(bank, rom.get_rom_bank());
        }
    }
}