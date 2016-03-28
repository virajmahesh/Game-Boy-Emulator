/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 */

#ifndef GAME_BOY_EMULATOR_CARTRIDGE_H
#define GAME_BOY_EMULATOR_CARTRIDGE_H


#include <cstdint>

#define address_between(x, y) (x <= address and address <= y)

#define ROM_TYPE_ADDR 0x0147
#define RAM_SIZE_ADDR 0x0149

#define ROM_BANK_SIZE 0x4000
#define RAM_BANK_SIZE 0x2000

using namespace std;

enum MBCType {
    ROM_ONLY, // 32 KB ROM.
    MBC1_16_8, // 16 Mb ROM, 8 KB RAM.
    MBC1_4_32, // 4 Mb ROM, 32 KB RAM.
    MBC2, // 2 Mb ROM, 2 Kb RAM.
    MBC3, // 16 Mb ROM, 8 KB RAM.
    MBC5, // 64 Mb ROM, 1 Mb RAM.
};

/*
 * Provides access to cartridge data (ROM and cartridge RAM if available).
 * Performs address translation before reading from or writing to the
 * cartridge.
 */
class Cartridge {

private:
    uint8_t *rom_data;
    uint8_t *ram_data;

    // Size of the ROM in bytes.
    int rom_size;
    int ram_size;

    // The ROM and RAM bank number.
    int rom_bank;
    int ram_bank;

    // The type of cartridge. This determines the memory ban controller.
    MBCType type;


public:

    Cartridge();

    /*
     * Create a new Cartridge.
     *
     * @param data: A byte array that contains ROM data.
     */
    Cartridge(uint8_t* data);

    /*
     * Create a new Cartridge.
     *
     * @param type: The type of memory bank controller used by the cartridge.
     * @param data: A byte array that contains the rom data.
     * @param rom_size: The size of the ROM in bytes.
     * @param ram_size: The size of cartridge RAM in bytes.
     */
    Cartridge(MBCType type, uint8_t* data, int rom_size, int ram_size);

    /*
     * Read a byte of data from ROM.
     *
     * @param address: The memory address to read from.
     * @return: The byte stored at this address.
     */
    virtual uint8_t load_byte_rom(uint16_t address);

    /*
     * Store a byte of data in ROM. Since the ROM is read-only, writing to the
     * ROM is used to trigger ROM or cartridge RAM bank switching.
     *
     * @param address: The memory address to write to.
     * @param value: The byte to be written.
     */
    virtual void store_byte_rom(uint16_t address, uint8_t value);

    /**
     * Read 2 bytes of data from ROM.
     *
     * @param address: The memory address to read from.
     * @return: The word stored at this address.
     */
    virtual uint16_t load_word_rom(uint16_t address);

    /*
     * Store 2 bytes of data in ROM. Since the ROM is read-only, writing to the
     * ROM is used to trigger ROM or cartridge RAM bank switching.
     *
     * @param address: The memory address to write to.
     * @param value: The word to be written.
     */
    virtual void store_word_rom(uint16_t address, uint16_t value);

    /*
     * Read a byte od data from cartridge RAM.
     *
     * @param address: The memory address to read from.
     * @return: The byte stored at this address.
     */
    virtual uint8_t load_byte_ram(uint16_t address);

    /*
     * Store a byte of data in cartridge RAM.
     *
     * @param address: The memory address to write to.
     * @param value: The byte to be written.
     */
    virtual void store_byte_ram(uint16_t address, uint8_t value);

    /*
     * Read 2 bytes of data from cartridge RAM.
     *
     * @param address: The memory address to read from.
     * @return: The word stored at this address.
     */
    virtual uint16_t load_word_ram(uint16_t address);

    /*
     * Store 2 bytes of data in cartridge RAM.
     *
     * @param address: The memory address to write to.
     * @param value: The word to be written.
     */
    virtual void store_word_ram(uint16_t address, uint16_t value);

    /*
     * @return: A reference to a byte of data stored in ROM.
     */
    uint8_t & get_byte_reference_rom(uint16_t address);

    /*
     * @return : A reference to a byte of data store in cartridge RAM.
     */
    uint8_t & get_byte_reference_ram(uint16_t);

    /*
     * Read a byte of data from ROM without any address translation or bank
     * switching.
     */
    uint8_t access_rom_data(int address);

    /*
     * Read a byte of data from cartridge RAM without any address translation
     * or bank switching.
     */
    uint8_t access_ram_data(int address);

    /*
     * @return: The index of the active ROM bank.
     */
    int get_rom_bank();

    /*
     * @return: The index of the active cartridge RAM bank.
     */
    int get_ram_bank();
};


#endif
