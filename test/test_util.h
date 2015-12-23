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
 * Generate a random word in the range [a, b)
 */
uint16_t random_word(uint16_t a, uint16_t b) {
    return static_cast<uint16_t>((rand() % (b - a)) + a);
}

/*
 * Return a random byte array of a particular size. Remember to deallocate
 * the byte array later.
 */
uint8_t* random_byte_array(uint32_t size) {
    uint8_t *byte_array = new uint8_t[size];
    for (uint32_t i = 0; i < size; i++) {
        byte_array[i] = random_byte();
    }
    return byte_array;
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