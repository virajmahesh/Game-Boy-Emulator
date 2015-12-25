/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 * Interface for the Game Boy's memory. Supports storing and loading data to
 * and from a memory address. Handles memory operations such as ROM bank switching.
 * All memory addresses are 16 bits.
 *
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include "cartridge.h"

using namespace std;

class Memory {

private:

    // Internal RAM
    uint8_t ram[0xFFFF + 1];

    // The Game boy cartridge we are currently playing
    Cartridge *cart;

public:

    /**
     * Initialize a new Memory object.
     */
    Memory();

    Memory(Cartridge *cart);

    /**
     * Read a byte from memory.
     */
    uint8_t load_byte(uint16_t);

    /**
     * Store a byte in memory.
     */
    void store_byte(uint16_t, uint8_t);

    /**
     * Read a word from memory.
     */
    uint16_t load_word(uint16_t);

    /**
     * Store a word in memory.
     */
    void store_word(uint16_t, uint16_t);
};


#endif
