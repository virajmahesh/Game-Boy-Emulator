#include "util.h"

uint32_t file_size(ifstream & file) {
    uint32_t length = 0;

    // Seek to the end of the file in order to count the number of bytes.
    file.seekg(0, file.end);
    length = (uint32_t) file.tellg();
    file.seekg(0, file.beg);

    return length;
}

uint8_t* read_file(ifstream & stream) {
    uint32_t rom_size = file_size(stream);
    uint8_t* data = new uint8_t[rom_size];

    stream.read((char *)data, rom_size);
    return data;
}

bool bit(uint8_t x, uint8_t n) {
    return ((x & (1 << n)) >> n);
}