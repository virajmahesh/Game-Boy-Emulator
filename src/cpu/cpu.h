/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#ifndef GAME_BOY_EMULATOR_CPU_H
#define GAME_BOY_EMULATOR_CPU_H


#include <cstdint>
#include <ostream>
#include <sstream>
#include <iomanip>

#include "carry.h"
#include "../memory/memory.h"
#include "../util/util.h"

#define TIMER_4096_MODE 0
#define TIMER_262144_MODE 1
#define TIMER_65536_MODE 2
#define TIMER_16384_MODE 3

/*
 * Provides methods for executing instructions, updating register values, and
 * handling interrupts.
 *
 * Example usage:
 *
 *  while (Emulator is running) {
 *      cpu.execute_next_instr();
 *
 *      // Graphics and sound processing here.
 *
 *      cpu.handler_interrupts();
 *  }
 *
 * The CPU also tracks the number of clock cycles that have elapsed, and
 * updates the appropriate timing registers as required.
 */
class CPU {

private:
    bool halted; // CPU is halted.
    bool ime_flag; // Master interrupt flag.

    uint64_t num_instructions; // The number of instructions executed.
    uint16_t total_cycles; // The total number of cycles the CPU has been executing for.

    int serial_cycles; // Number of cycles since the SB register was updated.
    int serial_bits; // The number of serial bits transferred.
    int div_cycles; // Number of cycles since the DIV register was last updated.
    int timer_cycles; // Number of cycles since the TIMA register was updated.

    Memory & memory; // The memory that the CPU reads from and writes to.

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
    void update_serial(uint32_t);
    uint8_t get_timer_mux(uint16_t, uint8_t);

public:
    // CPU registers.
    union {
        uint16_t AF;
        struct {
            union {
                uint8_t F;
                struct {
                    uint8_t padding: 4; // Padding
                    uint8_t C_: 1; // The carry flag.
                    uint8_t H_: 1; // The half carry flag.
                    uint8_t N_: 1; // The subtraction flag.
                    uint8_t Z_: 1; // The zero flag.
                };
            };
            uint8_t A;
        };
    };

    union {
        uint16_t BC;
        struct {
            uint8_t C;
            uint8_t B;
        };
    };

    union {
        uint16_t DE;
        struct {
            uint8_t E;
            uint8_t D;
        };
    };

    union {
        uint16_t HL;
        struct {
            uint8_t L;
            uint8_t H;
        };
    };

    uint16_t SP; // The stack pointer.
    uint16_t PC; // The program counter.

    /*
     * Create a new CPU.
     *
     * @param mem: The memory from which the CPU will read from and write to.
     */
    CPU(Memory & mem);

   /*
    * Read and execute the next instruction. This method does not update the
    * internal timer. It does not handle any interrupts.
    *
    * @return: The number of cycles the CPU used to execute the instruction.
    */
    uint32_t fetch_execute_instruction();

    /*
     * Read and execute the next instruction. This method also updates the
     * internal timer. It does not handle any interrupts. If the CPU has
     * been halted, this method does nothing.
     *
     * @return: The number of cycles the CPU used to execute the instruction.
     */
    uint32_t execute_next_instr();

    /*
     * @return: The CPU information (i.e register values) formatted as a string.
     * Useful for debugging.
     */
    string to_string();

    /*
     * Check if any enabled interrupts have been triggered and respond to them.
     * If an enabled interrupt has been triggered, then this method pushes the
     * PC on to the stack, and sets the PC to the address of the interrupt
     * handler. This also clears the interrupt flag.
     */
    void handle_interrupts();

    /*
     * @return: The number of instructions executed by the CPU.
     */
    long unsigned get_num_instructions();

    /*
     * Handle the memory flags that were raised after executing an intstruction.
     */
    void handle_memory_flags();
};

#endif
