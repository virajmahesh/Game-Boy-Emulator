/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 * A collection of utility functions for testing.
 */

#include <cstdint>
#include <cstdlib>

/**
 * Generate a random byte.
 */
uint8_t random_byte() {
    return static_cast<uint8_t>(rand());
}

/**
 * Generate a random word (2 bytes).
 */
uint16_t random_word() {
    return static_cast<uint16_t>(rand());
}

/*
 * Get the first (most significant) byte of a word.
 */
uint8_t high_byte(uint16_t word) {
    return static_cast<uint8_t>(word >> 8);
}

/**
 * Get the second (least significant) byte of a word.
 */
uint8_t low_byte(uint16_t word) {
    return static_cast<uint8_t>((word << 8) >> 8);
}