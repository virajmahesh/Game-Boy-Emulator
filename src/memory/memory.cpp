/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 */

#include "memory.h"

Memory::Memory() : Memory(nullptr) {

}

Memory::Memory(Cartridge * cart) {
    this->cart = cart;
}

uint8_t Memory::load_byte(uint16_t addr) {
    if (0x0000 <= addr and addr <= 0x7FFF) {
        return cart->load_byte_rom(addr);
    }
    else if (0xA000 <= addr and addr <= 0xBFFF) {
        return cart->load_byte_ram(addr);
    }
    return ram[addr];
}

void Memory::store_byte(uint16_t addr, uint8_t val) {
    if (0x0000 <= addr and addr <= 0x7FFF) {
        cart->store_byte_rom(addr, val);
    }
    else if (0xA000 <= addr and addr <= 0xBFFF) {
        cart->store_byte_ram(addr, val);
    }
    else {
        ram[addr] = val;
    }
}

uint16_t Memory::load_word(uint16_t addr) {
    if (0x0000 <= addr and addr <= 0x7FFF) {
        return cart->load_word_rom(addr);
    }
    else if (0xA000 <= addr and addr <= 0xBFFF) {
        return cart->load_word_ram(addr);
    }
    return *reinterpret_cast<uint16_t *>(ram + addr);
}

void Memory::store_word(uint16_t addr, uint16_t val) {
    if (0x0000 <= addr and addr <= 0x7FFF) {
        cart->store_word_rom(addr, val);
    }
    else if (0xA000 <= addr and addr <= 0xBFFF) {
        cart->store_word_ram(addr, val);
    }
    else {
        *reinterpret_cast<uint16_t *>(ram + addr) = val;
    }
}
