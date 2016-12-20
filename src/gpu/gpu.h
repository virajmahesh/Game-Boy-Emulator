/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#ifndef GAME_BOY_EMULATOR_GPU_H
#define GAME_BOY_EMULATOR_GPU_H

#include <GLFW/glfw3.h>

#include "sprite.h"
#include "../util/util.h"
#include "../memory/memory.h"

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

#define ROWS_PER_TILE 8
#define COLUMNS_PER_TILE 8
#define TILE_ROW_SIZE 2

#define NUM_SPRITES 40
#define SPRITE_SIZE 4 // Size of the sprite in bytes.
#define SPRITE_X_OFFSET 8
#define SPRITE_Y_OFFSET 16

#define BLACK Pixel(0, 0, 0, 0)
#define DARK_GREY Pixel(96,  96,  96,  0)
#define LIGHT_GREY Pixel(192, 192, 192, 0)
#define WHITE Pixel(255, 255, 255, 0)

/*
 * Represents a single pixel on the screen.
 */
struct Pixel {
    uint8_t b; // Blue value.
    uint8_t g; // Green value.
    uint8_t r; // Red value.
    uint8_t a; // Alpha (transparency) value.

    Pixel();

    Pixel(uint8_t b, uint8_t g, uint8_t r, uint8_t a);

    bool operator==(const Pixel & p);

    bool operator != (const Pixel & p);
};

/*
 * Different modes the GPU can be in.
 */
enum Mode {
    OAM_ACCESS,
    VRAM_ACCESS,
    H_BLANK,
    V_BLANK
};

/*
 * Simulates the behavior of the Game Boy's Graphics Processing Unit (GPU).
 * Provides methods to update the internal GPU state and render the Game Boy
 * screen.
 */
class GPU {

private:

    static int window_width;
    static int window_height;

    const uint16_t base_addresses[2] = {0x9800, 0x9C00};
    const uint16_t tile_addresses[2] = {0x9000, 0x8000};

    const Pixel colors[4] = {WHITE, LIGHT_GREY, DARK_GREY, BLACK};

    Pixel palette[4]; // The 4 colors that make up the palette.

    // The number of cycles tracked by the GPU.
    long cycles;

    // Reference to the Game Boy's memory module.
    Memory & memory;

    // The GLFW window on which the screen will be rendered.
    GLFWwindow *window;

    // The array of pixels that will be rendered to the screen.
    Pixel *buffer;

    // The current mode that the GPU is in.
    Mode mode;

    Pixel get_pixel(int x, int y);

    void set_pixel(int x, int y, Pixel & pixel);

    void draw_screen();
    void load_background_into_buffer();
    void load_window_into_buffer();
    void load_sprites_into_buffer();

    static void window_resized(GLFWwindow *window, int width, int height);

public:

    /*
     * Create a new GPU. This also creates a new window in which the Game Boy's
     * screen will be rendered.
     *
     * @param mem: The memory from which the GPU will read from and write to.
     */
    GPU(Memory &);

    /*
     * @return: A pointer to the GLFW window that the Game Boy screen is being
     * rendered to.
     */
    GLFWwindow* get_window();

    /*
     * Updates the internal GPU state and redraws the screen if necessary.
     *
     * @param cpu_cycles: The number of cpu cycles that have elapsed since the
     * screen was last rendered.
     */
    void render_screen(uint32_t cpu_cycles);

    /*
     * @return: True if the window created by the GPU is still open, otherwise
     * False.
     */
    bool window_open();

    /*
     * Computes the hash of the screen buffer. Used primarily in testing to check if the
     * screen has changed.
     * @return The 64-bit hash of the screen buffer.
     */
    uint64_t screen_hash();
};

#endif
