/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 */

#include "ram.h"

uint8_t RandomAccessMemory::load_byte(uint16_t addr) {
    return data[addr];
}

void RandomAccessMemory::store_byte(uint16_t addr, uint8_t val) {
    data[addr] = val;
}

uint16_t RandomAccessMemory::load_word(uint16_t addr) {
    return data[addr] | data[addr + 0x0001] << 8;
}

void RandomAccessMemory::store_word(uint16_t addr, uint16_t val) {
    data[addr] = (uint8_t)((val << 8) >> 8);
    data[addr + 0x0001] = (uint8_t)(val >> 8);
}
