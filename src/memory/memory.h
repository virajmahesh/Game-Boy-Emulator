/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
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

// Memory flags.
#define RESET_DIV_CYCLES_FLAG 0
#define RELOAD_TIMER_A_FLAG 1
#define RELOAD_TIMER_B_FLAG 2

#define address_between(x, y) (x <= address and address <= y)

#include <cstdint>
#include <iostream>

#include "cartridge.h"

using namespace std;

/*
 * Provides access to the entire address space of the Game Boy. Handles
 * reads and writes to the memory mapped registers and any special behavior
 * related to those registers.
 */
class Memory {

private:

    // Internal RAM. Includes VRAM and OAM memory.
    uint8_t ram[0xFFFF + 1];

    // Internal Flags. Used to communicate with the CPU.
    struct flags {
        bool reset_div_cycles;
        bool reload_timer_a;
        bool reload_timer_b;
    } flags;

    uint8_t new_timer_value;


public:

    // The Game boy cartridge that is currently loaded.
    Cartridge & cartridge;

    void initialize_registers();
    void copy_sprite_memory(uint8_t value);

    Memory(Cartridge& cart);

    /*
     * Read a byte of data from memory.
     *
     * @param address: The memory address to read from.
     * @return: The byte stored at this address.
     */
    virtual uint8_t load_byte(uint16_t address);

    /*
     * Store a byte in memory.
     *
     * @param address: The memory address to write to.
     * @param value: The byte to be written.
     */
    virtual void store_byte(uint16_t address, uint8_t value);

    /*
     * Read 2 bytes of data from memory.
     *
     * @param address: The memory address to read from.
     * @return: The word stored at this address.
     */
    virtual uint16_t load_word(uint16_t address);

    /*
     * Store 2 bytes in memory.
     *
     * @param address: The memory address to write to.
     * @param value: The word to be written.
     */
    virtual void store_word(uint16_t address, uint16_t value);

    /*
     * @return: A reference to the byte stored in memory.
     */
    virtual uint8_t & get_byte_reference(uint16_t address);

    /*
     * Copy size bytes of data from the Game Boy's memory to the destination
     * address.
     *
     * @param destination: The destination to copy the data to.
     * @param address: The memory address to start copying from.
     * @param size: The number of bytes to copy.
     */
    virtual void copy(void* destination, uint16_t address, int size);


    /*
     * Read and return the value of a memory flag.
     */
    bool get_flag(int f);

    /*
     * Set the value of a memory flag.
     */
    void set_flag(int f, bool value);

    /*
     * @return: The value that the timer should be set to after the timer overflow.
     * This is usually the value in the TMA register, but
     */
    uint8_t get_new_timer_value();

    /*
     * Sets the new timer value to be the value in the TMA register. The CPU should
     * call this function immediately after a timer overflow.
     */
    void set_new_timer_value();
};


#endif
