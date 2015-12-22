/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 * A collection of utility functions for testing.
 */

#include <cstdint>
#include <cstdlib>

uint8_t random_byte() {
    return static_cast<uint8_t>(rand());
}

uint16_t random_word() {
    return static_cast<uint16_t>(rand());
}

uint8_t high_byte(uint16_t word) {
    return static_cast<uint8_t>(word >> 8);
}

uint8_t low_byte(uint16_t word) {
    return static_cast<uint8_t>((word << 8) >> 8);
}