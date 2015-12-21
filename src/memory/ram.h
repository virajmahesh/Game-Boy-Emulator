/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#ifndef RAM_H
#define RAM_H

#include <cstdint>

class RandomAccessMemory {

private:
    // Allocate RAM for the entire address space just to be safe.
    uint8_t data[0xFFFF + 1];

public:

    /**
     * Read a byte from RAM at the given memory address.
     */
    uint8_t load_byte(uint16_t);

    /**
     * Store a byte in RAM at the given memory address.
     */
    void store_byte(uint16_t, uint8_t);

    /**
     * Read a word (2 bytes) from RAM at the given memory address.
     */
    uint16_t load_word(uint16_t);

    /**
     * Store a word (2 bytes) in RAM at the given memory address.
     */
    void store_word(uint16_t, uint16_t);
};

#endif
