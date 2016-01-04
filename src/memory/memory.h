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

#define P1 0xFF00
#define SB 0xFF01
#define SC 0xFF02
#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07
#define IF 0xFF0F
#define NR_10 0xFF10
#define NR_11 0xFF11
#define NR_12 0xFF12
#define NR_13 0xFF13
#define NR_14 0xFF14
#define NR_21 0xFF16
#define NR_22 0xFF17
#define NR_23 0xFF18
#define NR_24 0xFF19
#define NR_30 0xFF1A
#define NR_31 0xFF1B
#define NR_32 0xFF1C
#define NR_33 0xFF1D
#define NR_34 0xFF1E
#define NR_41 0xFF20
#define NR_42 0xFF21
#define NR_43 0xFF22
#define NR_44 0xFF23
#define NR_50 0xFF24
#define NR_51 0xFF25
#define NR_52 0xFF26
#define LCDC 0xFF40
#define STAT 0xFF41
#define SCY 0xFF42
#define SCX 0xFF43
#define LY 0xFF44
#define LYC 0xFF45
#define DMA 0xFF46
#define BGP 0xFF47
#define OBP0 0xFF48
#define OBP1 0xFF49
#define WY 0xFF4A
#define WX 0xFF4B
#define IE 0xFFFF


#include <cstdint>
#include <iostream>

#include "cartridge.h"

using namespace std;

class Memory {

private:

    // Internal RAM
    uint8_t ram[0xFFFF + 1];

    // The Game boy cartridge we are currently playing
    Cartridge *cart;

    void initialize_registers();

public:

    /**
     * Initialize a new Memory object.
     */
    Memory();

    Memory(Cartridge *cart);

    /**
     * Read a byte from memory.
     */
    virtual uint8_t load_byte(uint16_t);

    /**
     * Store a byte in memory.
     */
    virtual void store_byte(uint16_t, uint8_t);

    /**
     * Read a word from memory.
     */
    virtual uint16_t load_word(uint16_t);

    /**
     * Store a word in memory.
     */
    virtual void store_word(uint16_t, uint16_t);

    /*
     * Get a reference to a byte stored in memory.
     */
    virtual uint8_t & get_byte_reference(uint16_t);
};


#endif
