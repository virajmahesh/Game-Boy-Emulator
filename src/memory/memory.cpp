/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 */

#include "memory.h"
#include "../util/util.h"

Memory::Memory(Cartridge& cart) : cartridge(cart) {
    initialize_registers();
}

inline void Memory::initialize_registers() {
    memset(ram + 0xFF00, 0xFF, 0xFF);
    memset(&flags, false, sizeof(flags));

    ram[SB] = 0x00;
    ram[SC] = 0x7E;
    ram[TIMA] = 0x00;
    ram[TMA] = 0x00;
    ram[TAC] = 0xF8;
    ram[IF] = 0xE1;
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
    ram[NR_52] = 0xF0;
    ram[LCDC] = 0x91;
    ram[STAT] = 0x83;
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

uint8_t Memory::load_byte(uint16_t address) {
    if (address_between(0x0000, 0x7FFF)) {
        return cartridge.load_byte_rom(address);
    }
    else if (address_between(0xA000, 0xBFFF)) {
        return cartridge.load_byte_ram(address);
    }
    return ram[address];
}

void Memory::store_byte(uint16_t address, uint8_t val) {
    if (address_between(0x0000, 0x7FFF)) {
        cartridge.store_byte_rom(address, val);
    }
    else if (address_between(0xA000, 0xBFFF)) {
        cartridge.store_byte_ram(address, val);
    }
    else if (address == DIV) {
        ram[DIV] = 0;
        flags.reset_div_cycles = true;
        flags.reset_timer_cycles = true;
    }
    else if (address == TIMA) {
        if (flags.reload_timer_a) { // Overwrite new_timer_value.
            new_timer_value = val;
        }
        else if (flags.reload_timer_b) {
            // Ignore writes to TIMA in this state.
        }
        else {
            ram[TIMA] = val; // Normal behavior.
        }
    }
    else if (address == TMA) {
        if (flags.reload_timer_b) {
            ram[TIMA] = val;
        }
        ram[TMA] = val;
        new_timer_value = load_byte(TMA);
    }
    else if (address == IF) {
        ram[address] = 0xE0 | (val & 0x1F);
    }
    else if (address == DMA) {
        copy_sprite_memory(val);
    }
    else {
        ram[address] = val;
    }
}

uint16_t Memory::load_word(uint16_t address) {
    if (address_between(0x0000, 0x7FFF)) {
        return cartridge.load_word_rom(address);
    }
    else if (address_between(0xA000, 0xBFFF)) {
        return cartridge.load_word_ram(address);
    }
    return *reinterpret_cast<uint16_t *>(ram + address);
}

void Memory::store_word(uint16_t address, uint16_t value) {
    if (address_between(0x0000, 0x7FFF)) {
        cartridge.store_word_rom(address, value);
    }
    else if (address_between(0xA000, 0xBFFF)) {
        cartridge.store_word_ram(address, value);
    }
    else {
        *reinterpret_cast<uint16_t *>(ram + address) = value;
    }
}

uint8_t & Memory::get_byte_reference(uint16_t address) {
    if (address_between(0x0000, 0x7FFF)) {
        return cartridge.get_byte_reference_rom(address);
    }
    else if (address_between(0xA000, 0xBFFF)) {
        return cartridge.get_byte_reference_ram(address);
    }

    return ram[address];
}

void Memory::copy(void *dest, uint16_t addr, int size) {
    memcpy(dest, ram + addr, size);
}

inline void Memory::copy_sprite_memory(uint8_t value) {
    uint16_t cart_addr = value << 8;
    for (int i = 0; i < 0x9F; i++) {
        ram[0xFE00 + i] = load_byte(cart_addr + i);
    }
}

bool Memory::get_flag(int f) {
    return ((bool *)&flags)[f];
}

void Memory::set_flag(int f, bool value) {
    ((bool *)&flags)[f] = value;
}

void Memory::set_new_timer_value() {
    new_timer_value = load_byte(TMA);
}

uint8_t Memory::get_new_timer_value() {
    return new_timer_value;
}