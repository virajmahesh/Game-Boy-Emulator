/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 * Tests that the memory module correctly handles basic load and store operations.
 */

#include <cstdlib>
#include <ctime>
#include <gtest/gtest.h>


#include "../../src/memory/memory.h"
#include "../test_util.h"

using namespace testing;

/*
 * Test that the memory module correctly stores and loads a byte to and from
 * a given address.
 */
TEST(MemoryTest, LoadAndStoreByteWorks) {
    Memory memory;

    uint8_t data = random_byte();
    uint16_t address = random_word();

    memory.store_byte(address, data);

    // Test that the byte was stored correctly by reading it back.
    EXPECT_EQ(data, memory.load_byte(address));
}

/*
 * Test that the memory module correctly stores and loads a word to and from
 * a given address. Checks that words are stored with the least significant
 * byte first.
 */
TEST(MemoryTest, LoadAndStoreWordWorks) {
    Memory memory;

    uint16_t data = random_word();
    uint16_t address = random_word();

    memory.store_word(address, data);

    // Test that the word was stored correctly by reading the entire word.
    EXPECT_EQ(data, memory.load_word(address));

    // Test that the word was stored correctly by loading both bytes seperately.
    EXPECT_EQ(low_byte(data), memory.load_byte(address));
    EXPECT_EQ(high_byte(data), memory.load_byte(address + 0x0001));
}


int main(int argc, char **argv) {
    srand(time(NULL)); // Initialize random number generator

    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}