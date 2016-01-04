/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 * Interface for Game Boy ROMs. Supports cartridge types that have RAM chips.
 * Handles ROM and RAM bank switching. Loads and stores are made to the
 * correct ROM and RAM bank.
 */

#ifndef GAME_BOY_EMULATOR_CARTRIDGE_H
#define GAME_BOY_EMULATOR_CARTRIDGE_H


#include <cstdint>

using namespace std;

enum MBCType {
    ROM_ONLY, // 32 KB ROM
    MBC1_16_8, // 16 Mb ROM, 8 KB RAM
    MBC1_4_32, // 4 Mb ROM, 32 KB RAM
    MBC2, // 2 Mb ROM, 2 Kb RAM
    MBC3, // 16 Mb ROM, 8 KB RAM
    MBC5, // 64 Mb ROM, 1 Mb RAM
};

class Cartridge {
private:
    // Stores ROM data in memory.
    uint8_t *rom_data;
    uint8_t *ram_data;

    // Size of the ROM in bytes.
    uint32_t rom_size;
    uint32_t ram_size;

    // The ROM and RAM bank number.
    uint16_t rom_bank;
    uint16_t ram_bank;

    // The type of cartridge. This determines the memory ban controller.
    MBCType type;

    static const uint32_t ROM_TYPE_ADDR = 0x0147;
    static const uint32_t ROM_SIZE_ADDR = 0x0148;
    static const uint32_t RAM_SIZE_ADDR = 0x0149;

public:
    static const uint32_t ROM_BANK_SIZE = 0x4000;
    static const uint32_t RAM_BANK_SIZE = 0x2000;
    static const uint32_t MAX_BANKS = 128;
    static const uint32_t MAX_SIZE = MAX_BANKS * ROM_BANK_SIZE;

    Cartridge();

    /*
     * Create a new cartridge by passing in a byte array. This constructor will parse
     * size information and MBC type from the rom file.
     */
    Cartridge(uint8_t *);

    /*
     * Create a new cartridge by passing in a byte array and specifying the RAM size.
     */
    Cartridge(MBCType, uint8_t *, uint32_t, uint32_t);

    /**
     * Destroy a cartridge. Frees up all memory allocated by the constructor.
     */
    virtual ~Cartridge();

    /*
     * Read a byte from cartridge ROM.
     */
    virtual uint8_t load_byte_rom(uint16_t);

    /*
     * Store a byte in cartridge ROM.
     * This method should be used for bank switching.
     */
    virtual void store_byte_rom(uint16_t, uint8_t);

    /**
     * Read a word from ROM.
     */
    virtual uint16_t load_word_rom(uint16_t);

    /*
     * Store a word in cartridge ROM.
     * This method should be used for bank switching.
     */
    virtual void store_word_rom(uint16_t, uint16_t);

    /*
     * Read a byte from cartridge RAM.
     */
    virtual uint8_t load_byte_ram(uint16_t);

    /*
     * Store a byte in cartridge RAM.
     */
    virtual void store_byte_ram(uint16_t, uint8_t);

    /*
     * Read a word from cartridge RAM.
     */
    virtual uint16_t load_word_ram(uint16_t);

    /*
     * Store a word in cartridge RAM.
     */
    virtual void store_word_ram(uint16_t, uint16_t);

    /*
     * Get a reference to a byte of data store in ROM.
     */
    uint8_t & get_byte_reference_rom(uint16_t);

    /*
     * Get reference to a byte of data stored in RAM.
     */
    uint8_t & get_byte_reference_ram(uint16_t);

    /*
     * Directly access a byte from ROM data. Useful for testing.
     */
    uint8_t access_rom_data(uint32_t);

    /*
     * Directly access a byte from RAM data. Useful for testing.
     */
    uint8_t access_ram_data(uint32_t);

    /*
     * Get the current rom bank that is active.
     */
    uint16_t get_rom_bank();

    /*
     * Get the current ram bank that is active
     */
    uint16_t get_ram_bank();
};


#endif
