/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#include "util.h"

uint32_t file_size(ifstream & file) {
    uint32_t length = 0;

    file.seekg(0, file.end);
    length = (uint32_t) file.tellg();
    file.seekg(0, file.beg);

    return length;
}

uint8_t* read_file(ifstream & file) {
    uint32_t rom_size = file_size(file);
    uint8_t* data = new uint8_t[rom_size];

    file.read((char *)data, rom_size);
    return data;
}

long elapsed_time(time_t start_time) {
    return time(NULL) - start_time;
}