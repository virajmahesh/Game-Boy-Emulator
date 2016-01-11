/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#ifndef GAME_BOY_EMULATOR_UTIL_H
#define GAME_BOY_EMULATOR_UTIL_H

#include <fstream>
#include <cstdint>

using namespace std;

/*
 * @return: A byte array containing the contents of the stream.
 */
uint8_t* read_file(ifstream & stream);

/*
 * @return: The size of the file in bytes.
 */
uint32_t file_size(ifstream & file);

/*
 * @param x: The byte we are examining.
 * @param n: The bit index we are checking.
 * @return: The nth bit of the given byte.
 */
bool bit(uint8_t x, uint8_t n);

#endif
