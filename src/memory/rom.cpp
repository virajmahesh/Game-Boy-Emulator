/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#include "rom.h"

ROM::ROM(CartridgeType type) : ROM(type, nullptr) {

}

ROM::ROM(CartridgeType type, uint8_t * data) {
    rom_bank = 1;
    ram_bank = 0;
    this->type = type;
    this->data = data;
}

ROM::~ROM() {
    if (data != nullptr) {
        delete data;
    }
}

uint8_t ROM::load_byte_rom(uint16_t addr) {
    if (0x4000 <= addr and addr <= 0x7FFF) {
        return data[(BANK_SIZE * rom_bank) + addr - 0x4000];
    }
    return data[addr];
}

void ROM::store_byte_rom(uint16_t addr, uint8_t val) {
    switch (type) {
        case ROM_ONLY:
            break;

        case MBC1_16_8:
            if (0x2000 <= addr and addr <= 0x3FFF) {
                if (val == 0) {
                    val = 0x01;
                }
                rom_bank = (rom_bank & 0xE0) | (val & 0x1F);
            }
            if (0x4000 <= addr and addr <= 0x5FFF) {
                rom_bank = (rom_bank & 0x1F) | ((val & 0x03) << 5);
            }
            break;

        case MBC1_4_32:
            if (0x2000 <= addr and addr <= 0x3FFF) {
                if (val == 0) {
                    val = 0x01;
                }
                rom_bank = (rom_bank & 0xE0) | (val & 0x1F);
            }
            break;

        case MBC2:
            if (0x2000 <= addr and addr <= 0x3FFF) {
                if (val == 0) {
                    val = 0x01;
                }
                rom_bank = (val & 0x0F);
            }
            break;

        case MBC3:
            if (0x2000 <= addr and addr <= 0x3FFF) {
                if (val == 0) {
                    val = 0x01;
                }
                rom_bank = (val & 0x7F);
            }
            break;

        case MBC5:
            if (0x2000 <= addr and addr <= 0x2FFF) {
                rom_bank = (rom_bank & 0x0100) | (val & 0xFF);
            }
            if (0x3000 <= addr and addr <= 0x3FFF) {
                rom_bank = (rom_bank & 0x00FF) | (((uint16_t)(val & 0x01)) << 8);
            }
    }
}

uint16_t ROM::load_word_rom(uint16_t addr, uint8_t val) {
    // TODO: Implement me
    return 0;
}

void ROM::store_word_rom(uint16_t addr, uint16_t val) {
    // TODO: Implement me
}

uint8_t ROM::load_byte_ram(uint16_t addr) {
    // TODO: Implement me
    return 0;
}

void ROM::store_byte_ram(uint16_t addr, uint8_t val) {
    // TODO: Implement me
}

uint16_t ROM::load_word_ram(uint16_t addr) {
    // TODO: Implement me
    return 0;
}

void ROM::store_word_ram(uint16_t addr, uint16_t val) {
    // TODO: Implement me
}

uint8_t ROM::access_rom_data(uint32_t addr) {
    return data[addr];
}

uint16_t ROM::get_rom_bank() {
    return rom_bank;
}




