/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 */

#include "memory.h"

Memory::Memory() {

}

uint8_t Memory::load_byte(uint16_t addr) {
    return ram[addr];
}

void Memory::store_byte(uint16_t addr, uint8_t val) {
    ram[addr] = val;
}

uint16_t Memory::load_word(uint16_t addr) {
    return *reinterpret_cast<uint16_t *>(ram + addr);
}

void Memory::store_word(uint16_t addr, uint16_t val) {
    *reinterpret_cast<uint16_t *>(ram + addr) = val;
}
