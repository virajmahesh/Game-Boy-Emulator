/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#include "cpu.h"

CPU::CPU(Memory *memory) {
    AF = 0x11B0;
    BC = 0x0013;
    DE = 0x00D8;
    HL = 0x014D;
    SP = 0xFFFE;
    PC = 0x0100;
    ime_flag = false;
    this->memory = memory;
}

#define JR_d() int8_t d = memory->load_byte(PC + 1); PC += d + 2

uint8_t CPU::cc(uint8_t n) {
    switch (n) {
        case 0:
            return !Z_;
        case 1:
            return Z_;
        case 2:
            return !C_;
        case 3:
            return C_;
        default:
            throw "cc::Invalid flag index";
    }
}

uint16_t & CPU::rp(uint8_t n) {
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

uint16_t & CPU::rp2(uint8_t n) {
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

uint8_t & CPU::r(uint8_t n) {
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
            return memory->get_byte_reference(HL);
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
            C_ = (r(i) & 0x01);
            r(i) = (r(i) >> 1) | ((r(i) & 0x01) << 7);
            break;
        case 2:
            // RL r[i]
            N_ = 0;
            H_ = 0;
            r(i) = (r(i) << 1) | C_;
            C_ = old >> 7;
            break;
        case 3:
            // RR r[i]
            N_ = 0;
            H_ = 0;
            r(i) = r(i) >> 1 | (C_ << 7);
            C_ = old & 0x01;
            break;
        case 4:
            // SLA r[i]
            N_ = 0;
            H_ = 0;
            C_ = (r(i) & 0x80) >> 7;
            r(i) = (r(i) << 1) & 0xFE;
            break;
        case 5:
            // SRA r[i]
            N_ = 0;
            H_ = 0;
            C_ = r(i) & 0x01;
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
            C_ = r(i) & 0x01;
            r(i) = r(i) >> 1;
            break;
    }
    Z_ = (r(i) == 0)? 1 : 0;
}

inline void CPU::bit(uint8_t b, uint8_t z) {
    H_ = 1;
    N_ = 0;
    Z_ = !((r(z) & (0x01 << b)) >> b);
}

inline void CPU::res(uint8_t y, uint8_t z) {
    switch (y) {
        case 0:
            r(z) = r(z) & 0b11111110;
            break;
        case 1:
            r(z) = r(z) & 0b11111101;
            break;
        case 2:
            r(z) = r(z) & 0b11111011;
            break;
        case 3:
            r(z) = r(z) & 0b11110111;
            break;
        case 4:
            r(z) = r(z) & 0b11101111;
            break;
        case 5:
            r(z) = r(z) & 0b11011111;
            break;
        case 6:
            r(z) = r(z) & 0b10111111;
            break;
        case 7:
            r(z) = r(z) & 0b01111111;
            break;
        default:
            throw "Invalid bit index";
    }
}

inline void CPU::set(uint8_t y, uint8_t z) {
    switch (y) {
        case 0:
            r(z) = r(z) | 0b00000001;
            break;
        case 1:
            r(z) = r(z) | 0b00000010;
            break;
        case 2:
            r(z) = r(z) | 0b00000100;
            break;
        case 3:
            r(z) = r(z) | 0b00001000;
            break;
        case 4:
            r(z) = r(z) | 0b00010000;
            break;
        case 5:
            r(z) = r(z) | 0b00100000;
            break;
        case 6:
            r(z) = r(z) | 0b01000000;
            break;
        case 7:
            r(z) = r(z) | 0b10000000;
            break;
        default:
            throw "Invalid bit index";
    }
}

void CPU::execute_next_instr() {
    uint8_t instr = memory->load_byte(PC);
    uint8_t x = instr >> 6;
    uint8_t y = (instr & 0x38) >> 3;
    uint8_t z = instr & 0x07;
    uint8_t p = y >> 1;
    uint8_t q = y % 2;

    if (instr == 0xCB) {
        // CB Prefixed instructions
        instr = memory->load_byte(PC + 1);
        x = instr >> 6;
        y = (instr & 0x38) >> 3;
        z = instr & 0x07;
        if (x == 0) {
            rot(y, z);
            PC += 2;
        }
        else if (x == 1) {
            bit(y, z);
            PC += 2;
        }
        else if (x == 2) {
            res(y, z);
            PC += 2;
        }
        else if (x == 3) {
            set(y, z);
            PC += 2;
        }
    }
    else if (x == 0) {
        if (z == 0) {
            if (y == 0) {
                // NOP
                PC += 1;
            }
            else if (y == 1) {
                // LD (nn) SP
                uint16_t nn = memory->load_word(PC + 1);
                memory->store_word(nn, SP);
                PC += 3;
            }
            else if (y == 2) {
                // STOP
                PC += 2;
            }
            else if (y == 3) {
                // JR d
                JR_d();
            }
            else {
                // JR cc[y - 4], d
                if (cc(y - 4)) {
                    JR_d();
                }
                else {
                    PC += 2;
                }
            }
        }
        else if (z == 1) {
            if (q == 0) {
                // LD rp, nn
                uint16_t nn = memory->load_word(PC + 1);
                rp(p) = nn;
                PC += 3;
            }
            else if (q == 1) {
                // ADD HL, RP
                N_ = 0;
                H_ = half_carry_16(HL, rp(p));
                C_ = carry_16(HL, rp(p));
                HL += rp(p);
                PC += 1;
            }
        }
        else if (z == 2) {
            if (q == 0) {
                if (p == 0) {
                    // LD (BC), A
                    memory->store_byte(BC, A);
                    PC += 1;
                }
                else if (p == 1) {
                    // LD (DE), A
                    memory->store_byte(DE, A);
                    PC += 1;
                }
                else if (p == 2) {
                    // LD (HLI), A
                    memory->store_byte(HL, A);
                    HL += 1;
                    PC += 1;
                }
                else if (p == 3) {
                    // LD (HLD), A
                    memory->store_byte(HL, A);
                    HL -= 1;
                    PC += 1;
                }

            }
            else if (q == 1) {
                if (p == 0) {
                    // LD A, (BC)
                    A = memory->load_byte(BC);
                    PC += 1;
                }
                else if (p == 1) {
                    // LD A, (DE)
                    A = memory->load_byte(DE);
                    PC += 1;
                }
                else if (p == 2) {
                    // LD A, (HLI)
                    A = memory->load_byte(HL);
                    HL += 1;
                    PC += 1;
                }
                else if (p == 3) {
                    // LD A, (HLD)
                    A = memory->load_byte(HL);
                    HL -= 1;
                    PC += 1;
                }
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
        }
        else if (z == 4) {
            // INC r[y]
            N_ = 0;
            H_ = half_carry_8(r(y), 1);
            r(y) += 1;
            Z_ = (r(y) == 0)? 1: 0;
            PC += 1;

        }
        else if (z == 5) {
            // DEC r[y]
            N_ = 1;
            H_ = half_borrow_8(r(y), 1);
            r(y) -= 1;
            Z_ = (r(y) == 0)? 1: 0;
            PC += 1;
        }
        else if (z == 6) {
            // LD r[y], n
            r(y) = memory->load_byte(PC + 1);
            PC += 2;
        }
        else if (z == 7) {
            if (y <= 3) {
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

                A = (uint8_t)a;
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
        }
    }
    else if (x == 1) {
        // LD r[y], r[z]
        r(y) = r(z);
        PC += 1;
    }
    else if (x == 2) {
        // ALU[y] r[z]
        alu(y, r(z));
        PC += 1;
    }
    else if (x == 3) {
        if (z == 0) {
            if (instr == 0xE0) {
                // LD (0xFF00 + n), A ~ LDH n, A
                uint8_t n = memory->load_byte(PC + 1);
                memory->store_byte(0xFF00 + n, A);
                PC += 2;
            }
            else if (instr == 0xF0) {
                // LD A, (0xFF00 + n) ~ LDH A, n
                uint8_t n = memory->load_byte(PC + 1);
                A = memory->load_byte(0xFF00 + n);
                PC += 2;
            }
            else {
                // RET cc[y]
                if (y <  4 && cc(y)) {
                    PC = memory->load_word(SP);
                    SP += 2;
                }
                else if (instr == 0xF8) {
                    // LD HL SP + n
                    int8_t d = memory->load_byte(PC + 1);
                    N_ = 0;
                    Z_ = 0;
                    H_ = (((SP & 0x0F) + (d & 0x0F)) > 0x0F);
                    C_ = (((SP & 0xFF) + (d & 0xFF)) > 0xFF);
                    HL = SP + d;
                    PC += 2;
                }
                else if (instr == 0xE8) {
                    // ADD SP, n
                    int8_t d = memory->load_byte(PC + 1);
                    Z_ = 0;
                    N_ = 0;
                    H_ = (((SP & 0x0F) + (d & 0x0F)) > 0x0F);
                    C_ = (((SP & 0xFF) + (d & 0xFF)) > 0xFF);
                    SP += d;
                    PC += 2;
                }
                else {
                    PC += 1;
                }
            }
        }
        else if (z == 1) {
            if (q == 0) {
                // POP rp2[p]
                rp2(p) = memory->load_word(SP);
                SP += 2;
                PC += 1;
            }
            else if (q == 1) {
                if (p == 0) {
                    // RET
                    PC = memory->load_word(SP);
                    SP += 2;
                }
                else if (p == 1) {
                    // RETI
                    PC = memory->load_word(SP);
                    SP += 2;
                    ime_flag = true;
                }
                else if (p == 2) {
                    // JP HL
                    PC = HL;
                }
                else if (p == 3) {
                    // LD SP, HL
                    SP = HL;
                    PC += 1;
                }
            }
        }
        else if (z == 2) {
            if (y < 4) {
                // JP cc nn
                if (cc(y)) {
                    PC = memory->load_word(PC + 1);
                }
                else {
                    PC += 3;
                }
            }
            else if (instr == 0xEA){
                // LD (nn), A
                uint16_t nn = memory->load_word(PC + 1);
                memory->store_byte(nn, A);
                PC += 3;
            }
            else if (instr == 0xE2) {
                // LD (C), A
                memory->store_byte(0xFF00 + C, A);
                PC += 1;
            }
            else if (instr == 0xFA) {
                // LD A, (nn)
                uint16_t nn = memory->load_word(PC + 1);
                A = memory->load_byte(nn);
                PC += 3;
            }
            else if (instr == 0xF2) {
                // LD A, (C)
                A = memory->load_byte(0xFF00 + C);
                PC += 1;
            }
            else {
                // NOP
                PC += 1;
            }
        }
        else if (z == 3) {
            if (y == 0) {
                // JP nn
                PC = memory->load_word(PC + 1);
            }
            else if (y == 3) {
                PC += 1;
            }
            else if (y == 4) {
                PC += 1;
            }
            else if (y == 6) {
                // DI
                ime_flag = false;
                PC += 1;
            }
            else if (y == 7) {
                // EI
                ime_flag = true;
                PC += 1;
            }
        }
        else if (z == 4) {
            // CALL cc[y], nn
            if (y < 4 && cc(y)) {
                SP -= 2;
                memory->store_word(SP, PC + 3);
                PC = memory->load_word(PC + 1);
            }
            else {
                PC += 3;
            }

        }
        else if (z == 5) {
            if (q == 0) {
                // PUSH rp2[p]
                SP -= 2;
                memory->store_word(SP, rp2(p));
                PC += 1;
            }
            else if (q == 1) {
                if (p == 0) {
                    // CALL nn
                    SP -= 2;
                    memory->store_word(SP, PC + 3);
                    PC = memory->load_word(PC + 1);
                }
                else {
                    PC += 1;
                }
            }
        }
        else if (z == 6) {
            // ALU[y], n
            alu(y, memory->load_byte(PC + 1));
            PC += 2;
        }
        else if (z == 7) {
            // RST y*8
            SP -= 2;
            memory->store_word(SP, PC + 1);
            PC = y * 8;
        }
    }
    F = F & 0xF0; // Discard last 4 bits of F
    instructions += 1;

    // Update LCD
    // Update Timer
    // Update Serial

    // Handle the interrupts
    handle_interrupts();
 }

string CPU::to_string() {
    ostringstream out;

    uint16_t opcode = memory->load_byte(PC);
    uint16_t next_opcode = memory->load_byte(PC + 1);

    // Print the number of instructions executed so far
    out << instructions + 1 << ", ";

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

    return out.str();
}

uint64_t CPU::get_instructions() {
    return instructions;
}

void CPU::handle_interrupts() {
    uint8_t interrupts = memory->load_byte(IE) & memory->load_byte(IF);

    // Check if interrupts are enabled
    if (!interrupts || !ime_flag) {
        return;
    }

    // Push PC onto the stack
    SP -= 2;
    memory->store_word(SP, PC);
    ime_flag = false;

    if (interrupts & 0b00000001) {
        // V-Blank
        PC = 0x0040;
        memory->get_byte_reference(IF) &= 0b00011110;
    }
    else if (interrupts & 0b00000010) {
        // LCD-Stat
        PC = 0x0048;
        memory->get_byte_reference(IF) &= 0b00011101;
    }
    else if (interrupts & 0b00000100) {
        // Timer
        PC = 0x50;
        memory->get_byte_reference(IF) &= 0b00011011;
    }
    else if (interrupts & 0b00001000) {
        // Serial
        PC = 0x0058;
        memory->get_byte_reference(IF) &= 0b00010111;
    }
    else if (interrupts & 0b00010000) {
        // Serial
        PC = 0x0060;
        memory->get_byte_reference(IF) &= 0b00001111;
    }
}