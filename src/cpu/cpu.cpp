/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#include "cpu.h"


#define V_BLANK_INTERRUPT 0x01
#define LCDC_INTERRUPT    0x02
#define TIMER_INTERRUPT   0x04
#define SERIAL_INTERRUPT  0x08
#define JOYPAD_INTERRUPT  0x10

#define V_BLANK_ADDRESS 0x0040
#define LCDC_ADDRESS    0x0048
#define TIMER_ADDRESS   0x0050
#define SERIAL_ADDRESS  0x0058
#define JOYPAD_ADDRESS  0x0060

CPU::CPU(Memory & mem) : memory(mem) {
    // Initialize registers
    AF = 0x0100;
    BC = 0xFF13;
    DE = 0x00C1;
    HL = 0x8403;
    SP = 0xFFFE;
    PC = 0x0100;

    // Initialize cycle counters
    div_cycles = 0;
    timer_cycles = 0;

    // Initialize CPU state flags
    halted = false;
    ime_flag = false;
    num_instructions = 0;
}

inline uint8_t CPU::cc(uint8_t n) {
    switch (n) {
        case 0:
            return static_cast<uint8_t>(!Z_);
        case 1:
            return Z_;
        case 2:
            return static_cast<uint8_t>(!C_);
        case 3:
            return C_;
        default:
            throw "cc::Invalid flag index";
    }
}

inline uint16_t & CPU::rp(uint8_t n) {
    switch (n) {
        case 0:
            return BC;
        case 1:
            return DE;
        case 2:
            return HL;
        case 3:
            return SP;
        default:
            throw "rp::Invalid register pair index";
    }
}

inline uint16_t & CPU::rp2(uint8_t n) {
    switch (n) {
        case 0:
            return BC;
        case 1:
            return DE;
        case 2:
            return HL;
        case 3:
            return AF;
        default:
            throw "rp2::Invalid register pair index";
    }
}

inline uint8_t & CPU::r(uint8_t n) {
    switch (n) {
        case 0:
            return B;
        case 1:
            return C;
        case 2:
            return D;
        case 3:
            return E;
        case 4:
            return H;
        case 5:
            return L;
        case 6:
            return memory.get_byte_reference(HL);
        case 7:
            return A;
        default:
            throw "r::Invalid register index";
    }
}

inline void CPU::alu(uint8_t op, uint8_t val) {
    uint8_t old_C = C_;
    switch (op) {
        case 0:
            // ADD A, val
            N_ = 0;
            C_ = carry_8(A, val);
            H_ = half_carry_8(A, val);
            A += val;
            break;
        case 1:
            // ADC A, val
            N_ = 0;
            H_ = half_carry_8(A, val) or half_carry_8(A + val, C_);
            C_ = carry_8(A, val) or carry_8(A + val, C_);
            A += val + old_C;
            break;
        case 2:
            // SUB A, val
            N_ = 1;
            H_ = half_borrow_8(A, val);
            C_ = borrow_8(A, val);
            A -= val;
            break;
        case 3:
            // SUBC A, val
            N_ = 1;
            H_ = half_borrow_8(A, val) or half_borrow_8(A - val, C_);
            C_ = borrow_8(A, val) or borrow_8(A - val, C_);
            A -= (val + old_C);
            break;
        case 4:
            // AND A, val
            N_ = 0;
            H_ = 1;
            C_ = 0;
            A &= val;
            break;
        case 5:
            // XOR A, val
            N_ = 0;
            H_ = 0;
            C_ = 0;
            A ^= val;
            break;
        case 6:
            // OR A, val
            N_ = 0;
            H_ = 0;
            C_ = 0;
            A |= val;
            break;
        case 7:
            // CP val
            N_ = 1;
            H_ = half_borrow_8(A, val);
            C_ = borrow_8(A, val);

    }
    if (op != 7) {
        Z_ = (A == 0)? 1: 0;
    }
    else {
        Z_ = (A == val)?  1: 0;
    }
}

inline void CPU::rot(uint8_t op, uint8_t i) {
    uint8_t old = r(i);
    switch(op) {
        case 0:
            // RLC r[i]
            N_ = 0;
            H_ = 0;
            C_ = (r(i) >> 7);
            r(i) = (r(i) << 1) | C_;
            break;
        case 1:
            // RRC r[i]
            N_ = 0;
            H_ = 0;
            C_ = get_bit(r(i), 0);
            r(i) = (r(i) >> 1) | (get_bit(r(i), 0) << 7);
            break;
        case 2:
            // RL r[i]
            N_ = 0;
            H_ = 0;
            r(i) = (r(i) << 1) | C_;
            C_ = get_bit(old, 7);
            break;
        case 3:
            // RR r[i]
            N_ = 0;
            H_ = 0;
            r(i) = r(i) >> 1 | (C_ << 7);
            C_ = get_bit(old, 0);
            break;
        case 4:
            // SLA r[i]
            N_ = 0;
            H_ = 0;
            C_ = get_bit(r(i), 7);
            r(i) = (r(i) << 1);
            reset_bit(r(i), 0);
            break;
        case 5:
            // SRA r[i]
            N_ = 0;
            H_ = 0;
            C_ = get_bit(r(i), 0);
            r(i) = (r(i) & 0x80) | (r(i) >> 1);
            break;
        case 6:
            // SLL r[i]
            N_ = 0;
            H_ = 0;
            C_ = 0;
            r(i) = ((r(i) & 0x0F) << 4) | ((r(i) & 0xF0) >> 4);
            break;
        case 7:
            // SRL r[i]
            N_ = 0;
            H_ = 0;
            C_ = get_bit(r(i), 0);
            r(i) = r(i) >> 1;
            break;
    }
    Z_ = (r(i) == 0)? 1 : 0;
}

inline void CPU::bit(uint8_t b, uint8_t z) {
    H_ = 1;
    N_ = 0;
    Z_ = !get_bit(r(z), b);
}

inline void CPU::res(uint8_t y, uint8_t z) {
    switch (y) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            reset_bit(r(z), y);
            break;
        default:
            throw "Invalid bit index";
    }
}

inline void CPU::set(uint8_t y, uint8_t z) {
    switch (y) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            set_bit(r(z), y);
            break;
        default:
            throw "Invalid bit index";
    }
}

uint32_t CPU::execute_next_instr() {
    uint32_t cycles = 0;
    if (!halted) {
        cycles = fetch_execute_instruction();
    }
    else {
        // NOPs
        cycles += 4;
    }

    // Update timer and serial transfer registers.
    update_timer(cycles);
    update_serial(cycles);
    return cycles;
 }

string CPU::to_string() {
    ostringstream out;

    uint16_t opcode = memory.load_byte(PC);
    uint16_t next_opcode = memory.load_byte(PC + 1);

    // Print the number of instructions executed so far
    out << num_instructions + 1 << ", ";

    // Print the opcode.
    out << "Op: " << setfill('0') << setw(2) << hex << uppercase << opcode;
    out << " " << setfill('0') << setw(2) << hex << uppercase << next_opcode;

    // Print the CPU registers.
    out << ", PC: " << setfill('0') << setw(4) << hex << uppercase << PC;
    out << ", AF: " << setfill('0') << setw(4) << hex << uppercase << AF;
    out << ", BC: " << setfill('0') << setw(4) << hex << uppercase << BC;
    out << ", DE: " << setfill('0') << setw(4) << hex << uppercase << DE;
    out << ", HL: " << setfill('0') << setw(4) << hex << uppercase << HL;
    out << ", SP: " << setfill('0') << setw(4) << hex << uppercase << SP;
    out << ", H: " << halted;
    out << ", I: " << ime_flag;
    out << ", IF: " << static_cast<uint16_t>(memory.load_byte(IF));

    return out.str();
}

inline uint32_t CPU::fetch_execute_instruction() {
    uint8_t instr = memory.load_byte(PC);
    uint8_t x = instr >> 6; // Bits (6-7) of the instruction.
    uint8_t y = (instr & 0x38) >> 3; // Bits (3-5) of the instruction.
    uint8_t z = (instr & 0x07); // Bits (0-2) of the instruction.
    uint8_t p = y >> 1; // Bits (4-5) of the instruction.
    uint8_t q = y % 2; // Bit 3 of the instruction.

    uint32_t cycles = 0;

    if (instr == 0xCB) {
        // CB Prefixed instructions
        instr = memory.load_byte(PC + 1);
        x = instr >> 6;
        y = (instr & 0x38) >> 3;
        z = instr & 0x07;

        if (x == 0) {
            rot(y, z);
        }
        else if (x == 1) {
            bit(y, z);
        }
        else if (x == 2) {
            res(y, z);
        }
        else if (x == 3) {
            set(y, z);
        }
        cycles += 8;

        if (z == 6) {
            if (x == 1) {
                cycles += 4;
            }
            else {
                cycles += 8;
            }
        }

        PC += 2;
    }
    else if (x == 0) {
        if (z == 0) {
            if (y == 0) {
                // NOP
                PC += 1;
                cycles += 4;
            }
            else if (y == 1) {
                // LD (nn) SP
                uint16_t nn = memory.load_word(PC + 1);
                memory.store_word(nn, SP);
                PC += 3;
                cycles += 20;
            }
            else if (y == 2) {
                // STOP
                PC += 2;
                cycles += 4;
            }
            else if (y == 3) {
                // JR d
                int8_t d = memory.load_byte(PC + 1);
                PC += d + 2;
                cycles += 12;

            }
            else {
                // JR cc[y - 4], d
                if (cc(y - 4)) {
                    int8_t d = memory.load_byte(PC + 1);
                    PC += d + 2;
                    cycles += 12;
                }
                else {
                    PC += 2;
                    cycles += 8;
                }
            }
        }
        else if (z == 1) {
            if (q == 0) {
                // LD rp, nn
                uint16_t nn = memory.load_word(PC + 1);
                rp(p) = nn;
                PC += 3;
                cycles += 12;
            }
            else if (q == 1) {
                // ADD HL, RP
                N_ = 0;
                H_ = half_carry_16(HL, rp(p));
                C_ = carry_16(HL, rp(p));
                HL += rp(p);
                PC += 1;
                cycles += 8;
            }
        }
        else if (z == 2) {
            if (q == 0) {
                if (p == 0) {
                    // LD (BC), A
                    memory.store_byte(BC, A);
                    PC += 1;
                }
                else if (p == 1) {
                    // LD (DE), A
                    memory.store_byte(DE, A);
                    PC += 1;
                }
                else if (p == 2) {
                    // LD (HLI), A
                    memory.store_byte(HL, A);
                    HL += 1;
                    PC += 1;
                }
                else if (p == 3) {
                    // LD (HLD), A
                    memory.store_byte(HL, A);
                    HL -= 1;
                    PC += 1;
                }
                cycles += 8;
            }
            else if (q == 1) {
                if (p == 0) {
                    // LD A, (BC)
                    A = memory.load_byte(BC);
                    PC += 1;
                }
                else if (p == 1) {
                    // LD A, (DE)
                    A = memory.load_byte(DE);
                    PC += 1;
                }
                else if (p == 2) {
                    // LD A, (HLI)
                    A = memory.load_byte(HL);
                    HL += 1;
                    PC += 1;
                }
                else if (p == 3) {
                    // LD A, (HLD)
                    A = memory.load_byte(HL);
                    HL -= 1;
                    PC += 1;
                }
                cycles += 8;
            }
        }
        else if (z == 3) {
            if (q == 0) {
                // INC rp[p]
                rp(p) += 1;
                PC += 1;
            }
            else if (q == 1) {
                // DEC rp[p]
                rp(p) -= 1;
                PC += 1;
            }
            cycles += 8;
        }
        else if (z == 4) {
            // INC r[y]
            N_ = 0;
            H_ = half_carry_8(r(y), 1);
            r(y) += 1;
            Z_ = (r(y) == 0) ? 1 : 0;
            PC += 1;
            cycles += 4;
            if (y == 6) {
                cycles += 8;
            }
        }
        else if (z == 5) {
            // DEC r[y]
            N_ = 1;
            H_ = half_borrow_8(r(y), 1);
            r(y) -= 1;
            Z_ = (r(y) == 0) ? 1 : 0;
            PC += 1;
            cycles += 4;
            if (y == 6) {
                cycles += 8;
            }
        }
        else if (z == 6) {
            // LD r[y], n
            r(y) = memory.load_byte(PC + 1);
            PC += 2;
            cycles += 8;
            if (y == 6) {
                cycles += 4;
            }
        }
        else if (z == 7) {
            if (y <= 3) {
                // RRCA, RLCA, RRA, RLA
                rot(y, 7);
                Z_ = 0;
                PC += 1;
            }
            else if (y == 4) {
                // DAA
                int32_t a = A;
                if (N_ == 0) {
                    if (H_ || ((a & 0xF) > 9)) {
                        a += 0x06;
                    }
                    if (C_ || (a > 0x9F)) {
                        a += 0x60;
                    }
                }
                else {
                    if (H_) {
                        a = (a - 6) & 0xFF;
                    }
                    if (C_) {
                        a -= 0x60;
                    }
                }

                H_ = 0;
                Z_ = 0;

                if ((a & 0x100) == 0x100) {
                    C_ = 1;
                }

                a &= 0xFF;

                if (a == 0) {
                    Z_ = 1;
                }

                A = (uint8_t) a;
                PC += 1;
            }
            else if (y == 5) {
                // CPL
                A = ~A;
                N_ = 1;
                H_ = 1;
                PC += 1;
            }
            else if (y == 6) {
                // SCF
                C_ = 1;
                N_ = 0;
                H_ = 0;
                PC += 1;
            }
            else if (y == 7) {
                // CCF
                C_ = !C_;
                N_ = 0;
                H_ = 0;
                PC += 1;
            }
            cycles += 4;
        }
    }
    else if (x == 1) {
        if (instr == 0x76) {
            // HALT
            halted = true;
            PC += 1;
            cycles += 4;
        }
        else {
            // LD r[y], r[z]
            r(y) = r(z);
            PC += 1;
            cycles += 4;
            if (y == 6 or z == 6) {
                cycles += 4;
            }
        }
    }
    else if (x == 2) {
        // ALU[y] r[z]
        alu(y, r(z));
        PC += 1;
        cycles += 4;
        if (z == 6) {
            cycles += 4;
        }
    }
    else if (x == 3) {
        if (z == 0) {
            if (instr == 0xE0) {
                // LD (0xFF00 + n), A ~ LDH n, A
                uint8_t n = memory.load_byte(PC + 1);
                memory.store_byte(0xFF00 + n, A);
                PC += 2;
                cycles += 12;
            }
            else if (instr == 0xF0) {
                // LD A, (0xFF00 + n) ~ LDH A, n
                uint8_t n = memory.load_byte(PC + 1);
                A = memory.load_byte(0xFF00 + n);
                PC += 2;
                cycles += 12;
            }
            else {
                if (y < 4) {
                    // RET cc[y]
                    if (cc(y)) {
                        PC = memory.load_word(SP);
                        SP += 2;
                        cycles += 20;
                    }
                    else {
                        PC += 1;
                        cycles += 8;
                    }
                }
                else if (instr == 0xF8) {
                    // LD HL SP + n
                    int8_t d = memory.load_byte(PC + 1);
                    N_ = 0;
                    Z_ = 0;
                    H_ = (((SP & 0x0F) + (d & 0x0F)) > 0x0F);
                    C_ = (((SP & 0xFF) + (d & 0xFF)) > 0xFF);
                    HL = SP + d;
                    PC += 2;
                    cycles += 12;
                }
                else if (instr == 0xE8) {
                    // ADD SP, n
                    int8_t d = memory.load_byte(PC + 1);
                    Z_ = 0;
                    N_ = 0;
                    H_ = (((SP & 0x0F) + (d & 0x0F)) > 0x0F);
                    C_ = (((SP & 0xFF) + (d & 0xFF)) > 0xFF);
                    SP += d;
                    PC += 2;
                    cycles += 16;
                }
                else {
                    // NOP
                    PC += 1;
                    cycles += 4;
                }
            }
        }
        else if (z == 1) {
            if (q == 0) {
                // POP rp2[p]
                rp2(p) = memory.load_word(SP);
                SP += 2;
                PC += 1;
                cycles += 12;
            }
            else if (q == 1) {
                if (p == 0) {
                    // RET
                    PC = memory.load_word(SP);
                    SP += 2;
                    cycles += 16;
                }
                else if (p == 1) {
                    // RETI
                    PC = memory.load_word(SP);
                    SP += 2;
                    ime_flag = true;
                    cycles += 16;
                }
                else if (p == 2) {
                    // JP HL
                    PC = HL;
                    cycles += 4;
                }
                else if (p == 3) {
                    // LD SP, HL
                    SP = HL;
                    PC += 1;
                    cycles += 8;
                }
            }
        }
        else if (z == 2) {
            if (y < 4) {
                // JP cc nn
                if (cc(y)) {
                    PC = memory.load_word(PC + 1);
                    cycles += 16;
                }
                else {
                    PC += 3;
                    cycles += 12;
                }
            }
            else if (instr == 0xEA) {
                // LD (nn), A
                uint16_t nn = memory.load_word(PC + 1);
                memory.store_byte(nn, A);
                PC += 3;
                cycles += 16;
            }
            else if (instr == 0xE2) {
                // LD (C), A
                memory.store_byte(0xFF00 + C, A);
                PC += 1;
                cycles += 8;
            }
            else if (instr == 0xFA) {
                // LD A, (nn)
                uint16_t nn = memory.load_word(PC + 1);
                A = memory.load_byte(nn);
                PC += 3;
                cycles += 16;
            }
            else if (instr == 0xF2) {
                // LD A, (C)
                A = memory.load_byte(0xFF00 + C);
                PC += 1;
                cycles += 8;
            }
            else {
                // NOP
                PC += 1;
                cycles += 4;
            }
        }
        else if (z == 3) {
            if (y == 0) {
                // JP nn
                PC = memory.load_word(PC + 1);
                cycles += 16;
            }
            else if (y == 3) {
                PC += 1;
                cycles += 4;
            }
            else if (y == 4) {
                PC += 1;
                cycles += 4;
            }
            else if (y == 6) {
                // DI
                ime_flag = false;
                PC += 1;
                cycles += 4;
            }
            else if (y == 7) {
                // EI
                ime_flag = true;
                PC += 1;
                cycles += 4;
            }
        }
        else if (z == 4) {
            // CALL cc[y], nn
            if (y < 4) {
                if (cc(y)) {
                    SP -= 2;
                    memory.store_word(SP, PC + 3);
                    PC = memory.load_word(PC + 1);
                    cycles += 24;
                }
                else {
                    PC += 3;
                    cycles += 12;
                }
            }
            else {
                // NOP
                PC += 1;
                cycles += 4;
            }

        }
        else if (z == 5) {
            if (q == 0) {
                // PUSH rp2[p]
                SP -= 2;
                memory.store_word(SP, rp2(p));
                PC += 1;
                cycles += 16;
            }
            else if (q == 1) {
                if (p == 0) {
                    // CALL nn
                    SP -= 2;
                    memory.store_word(SP, PC + 3);
                    PC = memory.load_word(PC + 1);
                    cycles += 24;
                }
                else {
                    // NOP
                    PC += 1;
                    cycles += 4;
                }
            }
        }
        else if (z == 6) {
            // ALU[y], n
            alu(y, memory.load_byte(PC + 1));
            PC += 2;
            cycles += 8;
        }
        else if (z == 7) {
            // RST y*8
            SP -= 2;
            memory.store_word(SP, PC + 1);
            PC = y << 3;
            cycles += 16;
        }
    }

    F = F & 0xF0; // Discard last 4 bits of F
    num_instructions += 1;

    return cycles;
}

void CPU::handle_interrupts() {
    uint8_t interrupt_flag = memory.load_byte(IF);
    uint8_t interrupts = memory.load_byte(IE) & interrupt_flag;

    // Check if any of the enabled interrupts have fired.
    if (!interrupts) {
        return;
    }

    halted = false;

    if (!ime_flag) {
        return;
    }
    else {
        // Push PC onto the stack.
        SP -= 2;
        ime_flag = false;
        memory.store_word(SP, PC);
    }

    if (interrupts & V_BLANK_INTERRUPT) {
        // V-Blank interrupt.
        PC = V_BLANK_ADDRESS;
        reset_bit(interrupt_flag, 0);
    }
    else if (interrupts & LCDC_INTERRUPT) {
        // LCDC interrupt.
        PC = LCDC_ADDRESS;
        reset_bit(interrupt_flag, 1);
    }
    else if (interrupts & TIMER_INTERRUPT) {
        // Timer interrupt.
        PC = TIMER_ADDRESS;
        reset_bit(interrupt_flag, 2);
    }
    else if (interrupts & SERIAL_INTERRUPT) {
        // Serial interrupt.
        PC = SERIAL_ADDRESS;
        reset_bit(interrupt_flag, 3);
    }
    else if (interrupts & JOYPAD_INTERRUPT) {
        // Joypad interrupt.
        PC = JOYPAD_ADDRESS;
        reset_bit(interrupt_flag, 4);
    }

    memory.store_byte(IF, interrupt_flag);
}

inline void CPU::update_timer(uint32_t cycles) {
    timer_cycles += cycles;
    div_cycles += cycles;

    // Increment the div register.
    if (div_cycles > 256) {
        div_cycles -= 256;
        memory.get_byte_reference(DIV) += 1;
    }

    uint8_t tac = memory.load_byte(TAC);
    uint8_t timer = memory.load_byte(TIMA);
    uint8_t interrupt_flag = memory.load_byte(IF);

    uint32_t times[] = {1024, 16, 64, 256};
    uint32_t timer_threshold = times[tac & 0b00000011];

    // If timer is not enabled, then don't update it.
    if (!(tac >> 2)) {
        return;
    }

    while (timer_cycles >= timer_threshold) {
        if (timer == 0xFF) {
            timer = memory.load_byte(TMA);
            set_bit(interrupt_flag, 2); // Trigger a timer interrupt.
        }
        else {
            timer += 1;
        }
        timer_cycles -= timer_threshold;
    }

    memory.store_byte(TIMA, timer);
    memory.store_byte(IF, interrupt_flag);
}

inline void CPU::update_serial(uint32_t cycles) {
    uint8_t sc = memory.load_byte(SC);
    uint8_t sb = memory.load_byte(SB);
    uint8_t interrupt_flag = memory.load_byte(IF);

    if (get_bit(sc, 7) && get_bit(sc, 0)) {
        serial_cycles += cycles;

        if (serial_bits < 0) {
            serial_bits = 0;
            serial_cycles = 0;
        }
        else if (serial_cycles >= 512) {
            if (serial_bits > 7) {
                reset_bit(sc, 7);
                set_bit(interrupt_flag, 3);
                serial_bits = -1;
            }
            else {
                sb = sb << 1;
                set_bit(sb, 0);
                serial_cycles -= 512;
                serial_bits += 1;
            }
        }
    }

    memory.store_byte(SB, sb);
    memory.store_byte(SC, sc);
    memory.store_byte(IF, interrupt_flag);
}

long unsigned CPU::get_num_instructions() {
    return num_instructions;
}