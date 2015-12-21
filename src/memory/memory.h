/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 * Interface for the Game Boy's memory. Supports storing and loading data to
 * and from a memory address. Handles memory operations such as ROM bank switching.
 * All memory addresses are 16 bits.
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <exception>
#include <string>
#include "ram.h"

using namespace std;

class Memory {

private:

    RandomAccessMemory ram;

public:

    /**
     * Initialize a new Memory object.
     */
    Memory();

    /**
     * Read a byte from the given memory address.
     */
    uint8_t load_byte(uint16_t);

    /**
     * Store a byte at the given memory address.
     */
    void store_byte(uint16_t, uint8_t);

    /**
     * Read a word (2 bytes) from the given memory address.
     */
    uint16_t load_word(uint16_t);

    /**
     * Store a word (2 bytes) at the given memory address.
     */
    void store_word(uint16_t, uint16_t);
};


#endif
