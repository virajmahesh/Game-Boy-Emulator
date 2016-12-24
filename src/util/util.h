/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 *
 * Contains miscellaneous utility functions for file I/O, bit manipulation, etc.
 */

#ifndef GAME_BOY_EMULATOR_UTIL_H
#define GAME_BOY_EMULATOR_UTIL_H

#include <fstream>
#include <cstdint>

using namespace std;

/*
 * Clear the nth bit of a byte (i.e set it to 0).
 */
#define reset_bit(x, n) x &= ~(1 << n)

/*
 * Set the nth bit of a byte to 1.
 */
#define set_bit(x, n) x |= (1 << n)

/*
 * Get the nth bit of a byte.
 */
#define get_bit(x, n) ((x & (1 << (n))) >> (n))

/*
 * Return true if x is between a and b.
 */
#define between(a, x, b) ((a <= x) && (x <= b))

/*
 * @param file: An input stream for the file.
 * @return: The size of the file in bytes.
 */
uint32_t file_size(ifstream & file);

/*
 * Read an entire file into memory. Allocates a byte array large enough to
 * hold the contents of the file. It is the callers responsibility to
 * ensure that this buffer is deallocated when it is no longer required. The buffer
 * must be deallocated using `delete`.
 *
 * @param file: An input stream for the file.
 * @return: The contents of the file in a byte array.
 */
uint8_t* read_file(ifstream & file);

/*
 * @return: The time elapsed since start_time in seconds.
 */
long elapsed_time(time_t start_time);

#endif
