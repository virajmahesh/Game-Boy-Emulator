/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#include "memory.h"

Memory::Memory() {

}

uint8_t Memory::load_byte(uint16_t addr) {
  return ram.load_byte(addr);
}

void Memory::store_byte(uint16_t addr, uint8_t val) {
  return ram.store_byte(addr, val);
}

uint16_t Memory::load_word(uint16_t addr) {
  return ram.load_word(addr);
}

void Memory::store_word(uint16_t addr, uint16_t val) {
  return ram.store_word(addr, val);
}
