/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/cpu/cpu.h"

using namespace testing;

class MockMemory : public Memory {
public:
    MockMemory() : Memory(*static_cast<Cartridge *>(nullptr)) {};

    MOCK_METHOD1(load_byte, uint8_t(uint16_t));
    MOCK_METHOD2(store_byte, void(uint16_t, uint8_t));

    MOCK_METHOD1(load_word, uint16_t(uint16_t));
    MOCK_METHOD2(store_word, void(uint16_t, uint16_t));

    MOCK_METHOD1(get_byte_reference, uint8_t &(uint16_t));
};

/*
 * Test that the flag register works correctly.
 */
TEST(CPU_Test, Test_Flags) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    cpu.F = 0xFF;
    cpu.Z_ = 0;
    cpu.H_ = 0;
    cpu.C_ = 0;
    cpu.N_ = 0;

    EXPECT_EQ(0x0F, cpu.F);
}

/*
 * Test that NOP instructions works correctly.
 */
TEST(CPU_Test, Test_NOP) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x00));

    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x101, cpu.PC);
}

/*
 * Test that LD (nn), SP instruction works correctly.
 */
TEST(CPU_Test, Test_LD_nn_addr_SP) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x08));
    EXPECT_CALL(memory, load_word(0x0101)).Times(1).WillOnce(Return(0x1234));
    EXPECT_CALL(memory, store_word(0x1234, 0x5678)).Times(1);

    cpu.SP = 0x5678;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x100 + 3, cpu.PC);
}

/*
 * Test that JR d instruction works correctly.
 */
TEST(CPU_Test, Test_JR_d) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x18));
    EXPECT_CALL(memory, load_byte(0x0101)).Times(1).WillOnce(Return(0xFB));

    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x100 + 2 - 5, cpu.PC);

    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x18));
    EXPECT_CALL(memory, load_byte(0x0101)).Times(1).WillOnce(Return(0x05));

    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x100 + 2 + 5, cpu.PC);
}

/*
 * Test that JR cc, d instruction works correctly.
 */
TEST(CPU_Test, Test_JR_cc_d) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // JR NZ d
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x20));

    cpu.Z_ = 1;
    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x100 + 2, cpu.PC);

    // JR NC d
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x30));
    EXPECT_CALL(memory, load_byte(0x0101)).Times(1).WillOnce(Return(0x05));

    cpu.C_ = 0;
    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x100 + 2 + 5, cpu.PC);

    // JR Z d
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x28));
    EXPECT_CALL(memory, load_byte(0x0101)).Times(1).WillOnce(Return(0xFB));

    cpu.Z_ = 1;
    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x100 + 2 - 5, cpu.PC);

    // JR C d
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x38));

    cpu.C_ = 0;
    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x100 + 2, cpu.PC);
}

/*
 * Test that LD rp, nn instruction works correctly.
 */
TEST(CPU_Test, Test_LD_rp_nn) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // LD BC, nn
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x01));
    EXPECT_CALL(memory, load_word(0x0101)).Times(1).WillOnce(Return(0x1234));

    cpu.BC = 0x0000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x1234, cpu.BC);
    EXPECT_EQ(0x100 + 3, cpu.PC);

    // LD DE, nn
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x11));
    EXPECT_CALL(memory, load_word(0x0101)).Times(1).WillOnce(Return(0x1234));

    cpu.PC = 0x0100;
    cpu.DE = 0x0000;
    cpu.fetch_execute_instruction();

    // LD DE, nn
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x21));
    EXPECT_CALL(memory, load_word(0x0101)).Times(1).WillOnce(Return(0x1234));

    cpu.PC = 0x0100;
    cpu.HL = 0x0000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x1234, cpu.HL);
    EXPECT_EQ(0x100 + 3, cpu.PC);

    // LD SP, nn
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x31));
    EXPECT_CALL(memory, load_word(0x0101)).Times(1).WillOnce(Return(0x1234));

    cpu.PC = 0x0100;
    cpu.SP = 0x0000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x1234, cpu.SP);
    EXPECT_EQ(0x100 + 3, cpu.PC);
}

/*
 * Test that ADD HL, rp instruction works correctly.
 */
TEST(CPU_Test, ADD_HL_rp) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // ADD HL, BC
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x09));

    cpu.HL = 0x1880;
    cpu.BC = 0x0C00;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(1, cpu.H_);
    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0x2480, cpu.HL);
    EXPECT_EQ(0x100 + 1, cpu.PC);

    // ADD HL, DE
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x19));

    cpu.PC = 0x0100;
    cpu.HL = 0xF412;
    cpu.DE = 0x2412;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0x1824, cpu.HL);
    EXPECT_EQ(0x100 + 1, cpu.PC);

    // ADD HL, HL
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x29));

    cpu.PC = 0x0100;
    cpu.HL = 0x8800;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(1, cpu.H_);
    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0x1000, cpu.HL);
    EXPECT_EQ(0x100 + 1, cpu.PC);

    // ADD HL, SP
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x39));

    cpu.PC = 0x0100;
    cpu.HL = 0x8800;
    cpu.SP = 0x0000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0x8800, cpu.HL);
    EXPECT_EQ(0x100 + 1, cpu.PC);

}

/*
 * Test that LD (nn), A instruction works correctly.
 */
TEST(CPU_Test, LD_nn_addr_A) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // LD (nn), A
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xEA));
    EXPECT_CALL(memory, load_word(0x0100 + 1)).Times(1).WillOnce(Return(0xABCD));
    EXPECT_CALL(memory, store_byte(0xABCD, 0x12)).Times(1);

    cpu.A = 0x12;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x0100 + 3, cpu.PC);
}

/*
 * Test that JP nn instruction works correctly.
 */
TEST(CPU_Test, JP_nn) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // JP nn
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xC3));
    EXPECT_CALL(memory, load_word(0x0100 + 1)).Times(1).WillOnce(Return(0x4567));

    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x4567, cpu.PC);
}

/*
 * Test that LD r, n instruction works correctly.
 */
TEST(CPU_Test, LD_r_n) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // LD A, n
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x3E));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0xAB));

    cpu.fetch_execute_instruction();

    EXPECT_EQ(0xAB, cpu.A);
    EXPECT_EQ(0x0100 + 2, cpu.PC);

    // LD B, n
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x06));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0xCD));

    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0xCD, cpu.B);
    EXPECT_EQ(0x0100 + 2, cpu.PC);

    // LD C, n
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x0E));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0xEF));

    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0xEF, cpu.C);
    EXPECT_EQ(0x0100 + 2, cpu.PC);

    // LD D, n
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x16));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x12));

    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x12, cpu.D);
    EXPECT_EQ(0x0100 + 2, cpu.PC);

    // LD E, n
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x1E));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x34));

    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x34, cpu.E);
    EXPECT_EQ(0x0100 + 2, cpu.PC);

    // LD H, n
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x26));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x56));

    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x56, cpu.H);
    EXPECT_EQ(0x0100 + 2, cpu.PC);

    // LD L, n
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x2E));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x78));

    cpu.PC = 0x0100;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x78, cpu.L);
    EXPECT_EQ(0x0100 + 2, cpu.PC);

    // LD (HL), n
    uint8_t byte = 0xBC;
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0x36));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x9A));
    EXPECT_CALL(memory, get_byte_reference(0x1234)).Times(1).WillOnce(ReturnRef(byte));

    cpu.PC = 0x0100;
    cpu.HL = 0x1234;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x9A, byte);
    EXPECT_EQ(0x0100 + 2, cpu.PC);
}

/*
 * Test that LDH n, A instruction works correctly.
 */
TEST(CPU_Test, LDH_n_A) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // LDH n, A
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xE0));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x07));
    EXPECT_CALL(memory, store_byte(0xFF00 + 0x07, 0xAB)).Times(1);

    cpu.A = 0xAB;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x0100 + 2, cpu.PC);
}

/*
 * Test that CALL cc, nn instruction works correctly.
 */
TEST(CPU_Test, CALL_cc_nn) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // CALL NZ, nn
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xC4));
    EXPECT_CALL(memory, load_word(0x0100 + 1)).Times(1).WillOnce(Return(0x9988));
    EXPECT_CALL(memory, store_word(0xFFFE - 2, 0x0100 + 3)).Times(1);

    cpu.Z_ = 0;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x9988, cpu.PC);
    EXPECT_EQ(0xFFFE - 2, cpu.SP);

    // CALL NC, nn
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xD4));

    cpu.C_ = 1;
    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x0100 + 3, cpu.PC);
    EXPECT_EQ(0xFFFE, cpu.SP);

    // CALL Z, nn
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCC));

    cpu.Z_ = 0;
    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x0100 + 3, cpu.PC);
    EXPECT_EQ(0xFFFE, cpu.SP);

    // CALL C, nn
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xDC));
    EXPECT_CALL(memory, load_word(0x0100 + 1)).Times(1).WillOnce(Return(0xABCD));
    EXPECT_CALL(memory, store_word(0xFFFE - 2, 0x0100 + 3)).Times(1);

    cpu.C_ = 1;
    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0xABCD, cpu.PC);
    EXPECT_EQ(0xFFFE - 2, cpu.SP);
}

/*
 * Test that CALL nn instruction works correctly.
 */
TEST(CPU_Test, CALL_nn) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // CALL nn
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCD));
    EXPECT_CALL(memory, load_word(0x0100 + 1)).Times(1).WillOnce(Return(0x9988));
    EXPECT_CALL(memory, store_word(0xFFFE - 2, 0x0100 + 3)).Times(1);

    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x9988, cpu.PC);
    EXPECT_EQ(0xFFFE - 2, cpu.SP);
}

/*
 * Test that PUSH rp2 instruction works correctly.
 */
TEST(CPU_Test, PUSH_rp2) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // PUSH BC
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xC5));
    EXPECT_CALL(memory, store_word(0xFFFE - 2, 0x1234)).Times(1);

    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE;
    cpu.BC = 0x1234;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x0100 + 1, cpu.PC);
    EXPECT_EQ(0xFFFE - 2, cpu.SP);

    // PUSH DE
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xD5));
    EXPECT_CALL(memory, store_word(0xFFFE - 2, 0x5678)).Times(1);

    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE;
    cpu.DE = 0x5678;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x0100 + 1, cpu.PC);
    EXPECT_EQ(0xFFFE - 2, cpu.SP);

    // PUSH HL
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xE5));
    EXPECT_CALL(memory, store_word(0xFFFE - 2, 0x9ABC)).Times(1);

    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE;
    cpu.HL = 0x9ABC;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x0100 + 1, cpu.PC);
    EXPECT_EQ(0xFFFE - 2, cpu.SP);

    // PUSH AF
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xF5));
    EXPECT_CALL(memory, store_word(0xFFFE - 2, 0x1357)).Times(1);

    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE;
    cpu.AF = 0x1357;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x0100 + 1, cpu.PC);
    EXPECT_EQ(0xFFFE - 2, cpu.SP);
}

/*
 * Test that POP rp2 instruction works correctly.
 */
TEST(CPU_Test, POP_rp2) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // POP BC
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xC1));
    EXPECT_CALL(memory, load_word(0xFFFE - 2)).Times(1).WillOnce(Return(0x1234));

    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE - 2;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0xFFFE, cpu.SP);
    EXPECT_EQ(0x0100 + 1, cpu.PC);
    EXPECT_EQ(0x1234, cpu.BC);

    // POP DE
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xD1));
    EXPECT_CALL(memory, load_word(0xFFFE - 2)).Times(1).WillOnce(Return(0x5678));

    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE - 2;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0xFFFE, cpu.SP);
    EXPECT_EQ(0x0100 + 1, cpu.PC);
    EXPECT_EQ(0x5678, cpu.DE);

    // POP HL
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xE1));
    EXPECT_CALL(memory, load_word(0xFFFE - 2)).Times(1).WillOnce(Return(0xABCD));

    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE - 2;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0xFFFE, cpu.SP);
    EXPECT_EQ(0x0100 + 1, cpu.PC);
    EXPECT_EQ(0xABCD, cpu.HL);

    // POP AF
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xF1));
    EXPECT_CALL(memory, load_word(0xFFFE - 2)).Times(1).WillOnce(Return(0xFEE0));

    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE - 2;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0xFFFE, cpu.SP);
    EXPECT_EQ(0x0100 + 1, cpu.PC);
    EXPECT_EQ(0xFEE0, cpu.AF);
}

/*
 * Test that RET instruction works correctly.
 */
TEST(CPU_Test, RET) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xC9));
    EXPECT_CALL(memory, load_word(0xFFFE - 2)).Times(1).WillOnce(Return(0x1234));

    cpu.PC = 0x0100;
    cpu.SP = 0xFFFE - 2;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0xFFFE, cpu.SP);
    EXPECT_EQ(0x1234,  cpu.PC);
}

/*
 * Test that the LDH A, n instruction works correectly.
 */
TEST(CPU_Test, LDH_A_n) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xF0));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0xEF));
    EXPECT_CALL(memory, load_byte(0xFF00 + 0xEF)).Times(1).WillOnce(Return(0x99));

    cpu.fetch_execute_instruction();

    EXPECT_EQ(0x99,cpu.A);
    EXPECT_EQ(0x0100 + 2,  cpu.PC);
}

/*
 * Test that the RLC r instruction works correctly.
 */
TEST(CPU_Test, RLC_r) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // RLC B
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x00));

    cpu.B = 0b11010001;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b10100011, cpu.B);

    // RLC C
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x01));

    cpu.PC = 0x0100;
    cpu.C = 0b01010001;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b10100010, cpu.C);

    // RLC D
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x02));

    cpu.PC = 0x0100;
    cpu.D = 0b00000000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(1, cpu.Z_);
    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b00000000, cpu.D);

    // RLC E
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x03));

    cpu.PC = 0x0100;
    cpu.E = 0b10000000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b00000001, cpu.E);

    // RLC H
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x04));

    cpu.PC = 0x0100;
    cpu.H = 0b01000000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b10000000, cpu.H);

    // RLC L
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x05));

    cpu.PC = 0x0100;
    cpu.L = 0b10000000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b00000001, cpu.L);

    // RLC (HL)
    uint8_t byte = 0b01010101;
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x06));
    EXPECT_CALL(memory, get_byte_reference(0x1234)).WillRepeatedly(ReturnRef(byte));

    cpu.PC = 0x0100;
    cpu.HL = 0x1234;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b10101010, byte);

    // RLC A
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x07));

    cpu.PC = 0x0100;
    cpu.A = 0b00001111;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b00011110, cpu.A);
}

/*
 * Test that the RRC r instruction works correctly.
 */
TEST(CPU_Test, RRC_r) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // RRC B
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x08));

    cpu.B = 0b11010001;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b11101000, cpu.B);

    // RRC C
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x09));

    cpu.PC = 0x0100;
    cpu.C = 0b01010001;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b10101000, cpu.C);

    // RRC D
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x0A));

    cpu.PC = 0x0100;
    cpu.D = 0b00000000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(1, cpu.Z_);
    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b00000000, cpu.D);

    // RRC E
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x0B));

    cpu.PC = 0x0100;
    cpu.E = 0b10000000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b01000000, cpu.E);

    // RRC H
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x0C));

    cpu.PC = 0x0100;
    cpu.H = 0b01000000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b00100000, cpu.H);

    // RRC L
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x0D));

    cpu.PC = 0x0100;
    cpu.L = 0b10000000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b01000000, cpu.L);

    // RRC (HL)
    uint8_t byte = 0b01010101;
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x0E));
    EXPECT_CALL(memory, get_byte_reference(0x1234)).WillRepeatedly(ReturnRef(byte));

    cpu.PC = 0x0100;
    cpu.HL = 0x1234;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b10101010, byte);

    // RRC A
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x0F));

    cpu.PC = 0x0100;
    cpu.A = 0b00001111;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b10000111, cpu.A);
}

/*
 * Test that the RL r instruction works correctly.
 */
TEST(CPU_Test, RL_r) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // RL b
    cpu.C_ = 0;
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x10));

    cpu.B = 0b11101010;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(0b11010100, cpu.B);
    EXPECT_EQ(0x0100 + 2, cpu.PC);


    // RL C
    cpu.C_ = 1;
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x11));

    cpu.PC = 0x0100;
    cpu.C = 0b11101010;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(0b11010101, cpu.C);
    EXPECT_EQ(0x0100 + 2, cpu.PC);

    // RL D
    cpu.C_ = 0;
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x12));

    cpu.PC = 0x0100;
    cpu.D = 0b10000000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(1, cpu.C_);
    EXPECT_EQ(1, cpu.Z_);
    EXPECT_EQ(0b00000000, cpu.D);
    EXPECT_EQ(0x0100 + 2, cpu.PC);
}

/*
 * Test that the RR r instruction works correctly.
 */
TEST(CPU_Test, RR_r) {
    StrictMock<MockMemory> memory;
    CPU cpu = CPU(memory);

    // RR b
    cpu.C_ = 1;
    EXPECT_CALL(memory, load_byte(0x0100)).Times(1).WillOnce(Return(0xCB));
    EXPECT_CALL(memory, load_byte(0x0100 + 1)).Times(1).WillOnce(Return(0x18));

    cpu.B = 0b00010000;
    cpu.fetch_execute_instruction();

    EXPECT_EQ(0, cpu.C_);
    EXPECT_EQ(0, cpu.Z_);
    EXPECT_EQ(0, cpu.N_);
    EXPECT_EQ(0, cpu.H_);
    EXPECT_EQ(0b10001000, cpu.B);
    EXPECT_EQ(0x0100 + 2, cpu.PC);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}