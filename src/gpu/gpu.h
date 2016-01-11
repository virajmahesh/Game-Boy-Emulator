/*
 * Author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#ifndef GAME_BOY_EMULATOR_GPU_H
#define GAME_BOY_EMULATOR_GPU_H

#include <GLFW/glfw3.h>

#include "../util/util.h"
#include "../memory/memory.h"

#define TILE_MAP_0 0x9000
#define TILE_MAP_1 0x8000

#define OAM_ACCESS_TIME 80
#define VRAM_ACCESS_TIME 172
#define H_BLANK_TIME 204

#define V_BLANK_LINES 10

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

#define BGR_WIDTH 256
#define BGR_HEIGHT 256

#define TILE_SIZE 16 // Number of bytes per tile.
#define TILES_PER_ROW 32
#define TILES_PER_COLUMN 32

#define ROWS_PER_TILE 8
#define COLUMNS_PER_TILE 8

/*
 * Represents a single pixel on the screen.
 */
struct Pixel {
    uint8_t b; // Blue value
    uint8_t g; // Green value
    uint8_t r; // Red value.
    uint8_t a; // Alpha (transparency) value.
};

/*
 * Different modes the GPU can be in.
 */
enum GPU_Mode {
    OAM_ACCESS,
    VRAM_ACCESS,
    H_BLANK,
    V_BLANK
};

class GPU {

private:

    Pixel palette[4]; // The 4 colors that make up the palette.

    const Pixel COLORS[4] = {{255, 255, 255, 0}, {192, 192, 192, 0},
                             {96,  96,  96,  0}, {0, 0, 0, 0}};

    // The number of cycles tracked by the GPU.
    uint64_t cycles;

    // Reference to the Game Boy's memory module.
    Memory & memory;

    // The GLFW window on which the screen will be rendered.
    GLFWwindow *window;

    // The array of pixels that will be rendered to the screen.
    Pixel *buffer;

    // The current mode that the GPU is in.
    GPU_Mode mode;

    /*
     * Set the value of pixel (x, y).
     *
     * @param x: The x coordinate of the pixel.
     * @param y: The y coordinate of the pixel.
     * @param r: The new red value of the pixel.
     * @param g: The new green value of the pixel.
     * @param b: The new blue value of the pixel.
     * @param a: The new alpha value of the pixel.
     */
    void set_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    /*
     * Set the value of pixel (x, y).
     *
     * @param x: The x coordinate of the pixel.
     * @param y: The y coordinate of the pixel.
     * @param pixel: The pixel to be copied.
     */
    void set_pixel(uint32_t x, uint32_t y, Pixel & pixel);

    /*
     * Render the entire pixel buffer on the screen.
     */
    void draw_buffer();

    void load_background_into_buffer();

    void load_window_into_buffer();

    void load_sprites_into_buffer();

public:

    GPU(Memory &);

    /*
     * Redraw the Game Boy screen.
     *
     * @param cpu_cycles: The number of cycles the CPU has executed so far.
     */
    void render_screen(uint32_t cpu_cycles);
};

#endif
