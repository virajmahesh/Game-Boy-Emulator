/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 * Interface for the Game Boy's CPU. Fetches an instruction from memory and
 * executes it. At the end of the fetch-execute cycle, the CPU updates registers
 * and handles interrupts if required.
 */

#ifndef GAME_BOY_EMULATOR_CPU_H
#define GAME_BOY_EMULATOR_CPU_H


#include <cstdint>
#include <ostream>
#include <sstream>
#include <iomanip>

#include "carry.h"
#include "../memory/memory.h"

typedef uint16_t register16_t;
typedef uint8_t register8_t;

class CPU {

private:
    static const uint64_t CYCLES_PER_SEC = 4194304;

    bool halted; // CPU is halted (i.e timer is stopped).
    bool ime_flag; // Master interrupt flag.

    uint64_t num_instructions; // The number of instructions executed.
    uint64_t timer_cycles; // Number of cycles since TIMA register was updated.
    uint64_t div_cycles; // Number of cycles since the DIV register was last updated.

    Memory *memory; // The Memory that the CPU reads and writes to.

    uint8_t cc(uint8_t);
    uint8_t & r(uint8_t);
    uint16_t & rp(uint8_t);
    uint16_t & rp2(uint8_t);

    void alu(uint8_t, uint8_t);
    void rot(uint8_t, uint8_t);
    void bit(uint8_t, uint8_t);
    void res(uint8_t, uint8_t);
    void set(uint8_t, uint8_t);

    void update_timer(uint32_t);

public:
    union {
        register16_t AF;
        struct {
            union {
                register8_t F;
                struct {
                    uint8_t padding: 4; // Padding
                    uint8_t C_: 1;
                    uint8_t H_: 1;
                    uint8_t N_: 1;
                    uint8_t Z_: 1;
                };
            };
            register8_t A;
        };
    };
    union {
        register16_t BC;
        struct {
            register8_t C;
            register8_t B;
        };
    };
    union {
        register16_t DE;
        struct {
            register8_t E;
            register8_t D;
        };
    };
    union {
        register16_t HL;
        struct {
            register8_t L;
            register8_t H;
        };
    };
    register16_t SP;
    register16_t PC;

    CPU(Memory *);

    /*
     * Read and execute the next instruction from memory.
     */
    uint32_t execute_next_instr();

    uint32_t fetch_execute_instruction();

    /*
     * @return: Return the CPU registers formatted as strings. Useful for debugging.
     */
    string to_string();

    /*
     * @return: The number of instructions executed by the CPU.
     */
    uint64_t get_num_instructions();

    /*
     * Check interrupts and handle them.
     */
    void handle_interrupts();
};

#endif
