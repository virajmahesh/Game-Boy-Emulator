/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 */

#include "cartridge.h"

Cartridge::Cartridge() : Cartridge(ROM_ONLY, nullptr, 0, 0) {

}

Cartridge::Cartridge(uint8_t* data) {
    rom_data = data;
    // Parse MBC type
    switch (data[ROM_TYPE_ADDR]) {
        case 0x00:
        case 0x08:
        case 0x09:
        case 0x0B:
        case 0x0C:
        case 0x0D:
            type = ROM_ONLY;
            break;
        case 0x01:
        case 0x02:
        case 0x03:
            type = MBC1_16_8; // Correct mode should be set by the ROM
            break;
        case 0x05:
        case 0x06:
            type = MBC2;
            break;
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            type = MBC3;
            break;
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
            type = MBC5;
            break;
        default:
            throw "Unidentifiable ROM type";

    }

    // Allocate maximum possible amount of RAM.
    ram_size = RAM_BANK_SIZE * 16;
    ram_data = new uint8_t[ram_size];

    rom_bank = 1;
    ram_bank = 0;
}

Cartridge::Cartridge(MBCType type, uint8_t* data, int rom_size, int ram_size) {
    this->rom_bank = 1;
    this->ram_bank = 0;
    this->type = type;
    this->rom_data = data;
    this->rom_size = rom_size;
    this->ram_size = ram_size;

    if (this->ram_size > 0) {
        this->ram_data = new uint8_t[ram_size];
    }
    else {
        this->ram_data = nullptr;
    }
}

uint8_t Cartridge::load_byte_rom(uint16_t address) {
    if (address_between(0x4000, 0x7FFF)) {
        return rom_data[(ROM_BANK_SIZE * rom_bank) + address - 0x4000];
    }
    return rom_data[address];
}

void Cartridge::store_byte_rom(uint16_t address, uint8_t value) {
    switch (type) {
        case ROM_ONLY:
            break;

        case MBC1_16_8:
            if (address_between(0x2000, 0x3FFF)) {
                if (value == 0) {
                    value = 0x01;
                }
                rom_bank = (rom_bank & 0xE0) | (value & 0x1F);
            }
            if (address_between(0x4000, 0x5FFF)) {
                rom_bank = (rom_bank & 0x1F) | ((value & 0x03) << 5);
            }
            break;

        case MBC1_4_32:
            if (address_between(0x2000, 0x3FFF)) {
                if (value == 0) {
                    value = 0x01;
                }
                rom_bank = (rom_bank & 0xE0) | (value & 0x1F);
            }
            if (address_between(0x4000, 0x5FFF)) {
                ram_bank = value & 0x0F;
            }
            break;

        case MBC2:
            if (address_between(0x2000, 0x3FFF)) {
                if (value == 0) {
                    value = 0x01;
                }
                rom_bank = (value & 0x0F);
            }
            break;

        case MBC3:
            if (address_between(0x2000, 0x3FFF)) {
                if (value == 0) {
                    value = 0x01;
                }
                rom_bank = (value & 0x7F);
            }
            if (address_between(0x4000, 0x5FFF)) {
                ram_bank = value & 0x0F;
            }
            break;

        case MBC5:
            if (address_between(0x2000, 0x2FFF)) {
                rom_bank = (rom_bank & 0x0100) | (value & 0xFF);
            }
            if (address_between(0x3000, 0x3FFF)) {
                rom_bank = (rom_bank & 0x00FF) | (((uint16_t)(value & 0x01)) << 8);
            }
            if (address_between(0x4000, 0x5FFF)) {
                ram_bank = value & 0x0F;
            }
    }
}

uint16_t Cartridge::load_word_rom(uint16_t addr) {
    return *reinterpret_cast<uint16_t *>(rom_data + addr);
}

void Cartridge::store_word_rom(uint16_t addr, uint16_t val) {
    // TODO: Implement me
}

uint8_t Cartridge::load_byte_ram(uint16_t addr) {
    return ram_data[(RAM_BANK_SIZE * ram_bank) + addr - 0xA000];
}

void Cartridge::store_byte_ram(uint16_t addr, uint8_t val) {
    if (type == MBC2) {
        ram_data[(RAM_BANK_SIZE * ram_bank) + addr - 0xA000] = val & 0x0F;
    }
    else if (ram_data != nullptr){
        ram_data[(RAM_BANK_SIZE * ram_bank) + addr - 0xA000] = val;
    }
}

uint8_t & Cartridge::get_byte_reference_rom(uint16_t address) {
    if (address_between(0x4000, 0x7FFF)) {
        return rom_data[(ROM_BANK_SIZE * rom_bank) + address - 0x4000];
    }
    return rom_data[address];
}

uint8_t & Cartridge::get_byte_reference_ram(uint16_t addr) {
    return ram_data[(RAM_BANK_SIZE * ram_bank) + addr - 0xA000];
}

uint16_t Cartridge::load_word_ram(uint16_t addr) {
    return *reinterpret_cast<uint16_t *>(ram_data + addr);
}

void Cartridge::store_word_ram(uint16_t addr, uint16_t val) {
    *reinterpret_cast<uint16_t *>(ram_data + addr) = val;
}

uint8_t Cartridge::access_rom_data(int address) {
    return rom_data[address];
}

uint8_t Cartridge::access_ram_data(int address) {
    return ram_data[address];
}

int Cartridge::get_rom_bank() {
    return rom_bank;
}

int Cartridge::get_ram_bank() {
    return ram_bank;
}




