/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#include "ram.h"

uint8_t RandomAccessMemory::load_byte(uint16_t addr) {
  return data[addr];
}

void RandomAccessMemory::store_byte(uint16_t addr, uint8_t val) {
  data[addr] = val;
}

uint16_t RandomAccessMemory::load_word(uint16_t addr) {
  return ((uint16_t *)data)[addr];
}

void RandomAccessMemory::store_word(uint16_t addr, uint16_t val) {
  ((uint16_t *)data)[addr] = val;
}
