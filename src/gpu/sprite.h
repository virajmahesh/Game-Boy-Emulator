/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#ifndef GAME_BOY_EMULATOR_SPRITE_H
#define GAME_BOY_EMULATOR_SPRITE_H

#include <cstdint>

/*
 * Sprite information.
 */
struct Sprite {
    uint8_t y;
    uint8_t x;
    uint8_t tile_idx;
    union {
        uint8_t options;
        struct {
            uint8_t padding: 3;
            uint8_t palette: 1;
            uint8_t x_flip: 1;
            uint8_t y_flip: 1;
            uint8_t sprite_priority: 1;
        };
    };
};

#endif
