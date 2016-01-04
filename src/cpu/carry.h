/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#ifndef GAME_BOY_EMULATOR_CARRY_H
#define GAME_BOY_EMULATOR_CARRY_H

#include <cstdint>

uint8_t inline half_carry_16(uint16_t x, uint16_t y) {
    return (((x & 0xFFF) + (y & 0xFFF)) & 0x1000) >> 12;
}

uint8_t inline carry_16(uint16_t x, uint16_t y) {
    uint32_t result = x + y;
    return (uint8_t)(result > 0xFFFF);
}

uint8_t inline carry_8(uint8_t x, uint8_t y) {
    uint32_t result = x + y;
    return (uint8_t)(result > 0xFF);
}

/*
 * Checks if there is a carry from bit 3 to bit 4 (i.e from the low nibble to
 * the high nibble).
 *
 * @return 1 if there is a carry, otherwise 0.
 */
uint8_t inline half_carry_8(uint8_t x, uint8_t y) {
    return (((x & 0xF) + (y & 0xF)) & 0x10) >> 4;
}

uint8_t inline half_borrow_8(uint8_t x, uint8_t y) {
    return (x & 0x0F) < (y & 0x0F);
}

uint8_t inline borrow_8(uint8_t x, uint8_t y) {
    return x < y;
}

#endif
