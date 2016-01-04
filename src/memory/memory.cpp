/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 */

#include "memory.h"

Memory::Memory() : Memory(nullptr) {

}

Memory::Memory(Cartridge * cart) {
    this->cart = cart;
    initialize_registers();
}

inline void Memory::initialize_registers() {
    ram[TIMA] = 0x00;
    ram[TMA] = 0x00;
    ram[TAC] = 0x00;
    ram[NR_10] = 0x80;
    ram[NR_11] = 0xBF;
    ram[NR_12] = 0xF3;
    ram[NR_14] = 0xBF;
    ram[NR_21] = 0x3F;
    ram[NR_22] = 0x00;
    ram[NR_24] = 0xBF;
    ram[NR_30] = 0x7F;
    ram[NR_31] = 0xFF;
    ram[NR_32] = 0x9F;
    ram[NR_34] = 0xBF;
    ram[NR_41] = 0xFF;
    ram[NR_42] = 0x00;
    ram[NR_43] = 0x00;
    ram[NR_44] = 0xBF;
    ram[NR_50] = 0x77;
    ram[NR_51] = 0xF3;
    ram[NR_52] = 0xF1;
    ram[LCDC] = 0x91;
    ram[SCY] = 0x00;
    ram[SCX] = 0x00;
    ram[LY] = 0x90;
    ram[LYC] = 0x00;
    ram[BGP] = 0xFC;
    ram[OBP0] = 0xFF;
    ram[OBP1] = 0xFF;
    ram[WY] = 0x00;
    ram[WX] = 0x00;
    ram[IE] = 0x00;
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
    else if (addr == 0xFF02) {
        cout << (char)load_byte(0xFF01);
        cout.flush();
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

uint8_t & Memory::get_byte_reference(uint16_t addr) {
    if (0x0000 <= addr and addr <= 0x7FFF) {
        return cart->get_byte_reference_rom(addr);
    }
    else if (0xA000 <= addr and addr <= 0xBFFF) {
        return cart->get_byte_reference_ram(addr);
    }
    return ram[addr];
}
