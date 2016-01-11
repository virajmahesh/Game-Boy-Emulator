/*
 * Author: Viraj Mahesh
 */

#include <GLUT/glut.h>
#include "gpu.h"

GPU::GPU(Memory & mem) : memory(mem) {
    cycles = 0;
    mode = OAM_ACCESS;

    // Initialize GLFW and create a new window
    glfwInit();
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game Boy Emulator", NULL, NULL);

    glfwSwapInterval(2);
    glfwMakeContextCurrent(window);

    // Create a new buffer of pixels and initialize the screen to be white.
    buffer = new Pixel[SCREEN_WIDTH * SCREEN_HEIGHT];
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        buffer[i].a = 0;
        buffer[i].r = 255;
        buffer[i].g = 255;
        buffer[i].b = 255;
    }
}

void GPU::render_screen(uint32_t cpu_cycles) {
    switch (mode) {
        case OAM_ACCESS:
            if (cycles > OAM_ACCESS_TIME) {
                mode = VRAM_ACCESS;
                cycles -= OAM_ACCESS_TIME;
            }
            break;
        case VRAM_ACCESS:
            if (cycles > VRAM_ACCESS_TIME) {
                mode = H_BLANK;
                cycles -= VRAM_ACCESS_TIME;
                // Trigger an H-Blank interrupt if required
            }
            break;
        case H_BLANK:
            if (cycles > H_BLANK_TIME) {
                memory.get_byte_reference(LY) += 1;
                if (memory.load_byte(LY) == SCREEN_HEIGHT - 1) {
                    mode = V_BLANK;
                    // Render the pixel buffer on the screen, and trigger a V-Blank interrupt.
                    draw_buffer();
                    memory.get_byte_reference(IF) |= 0b00000001;
                }
                else {
                    mode = OAM_ACCESS;
                }
                cycles -= H_BLANK_TIME;
            }
        case V_BLANK: {
            if (cycles > H_BLANK_TIME) {
                memory.get_byte_reference(LY) += 1;
                if (memory.load_byte(LY) == SCREEN_HEIGHT + V_BLANK_LINES - 1) {
                    mode = OAM_ACCESS;
                    memory.get_byte_reference(LY) = 0;
                }
                cycles -= H_BLANK_TIME;
            }
        }
    }
    cycles += cpu_cycles;
}

void GPU::set_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    // Find the right pixel.
    Pixel *p = buffer + x + (y * SCREEN_WIDTH);

    // Set the pixel value.
    p->r = r;
    p->g = g;
    p->b = b;
    p->a = a;
}

void GPU::set_pixel(uint32_t x, uint32_t y, Pixel & pixel) {
    // Find the right pixel.
    y = SCREEN_HEIGHT - y;
    Pixel *p = buffer + x + (y * SCREEN_WIDTH);

    // Set the pixel value.
    p->r = pixel.r;
    p->g = pixel.g;
    p->b = pixel.b;
    p->a = pixel.a;
}

void GPU::load_background_into_buffer() {
    uint8_t lcdc = memory.load_byte(LCDC);

    if (!bit(lcdc, 0)) {
        return;
    }

    uint8_t scx = memory.load_byte(SCX);
    uint8_t scy = memory.load_byte(SCY);

    uint16_t base_address = (bit(lcdc, 3) == 1)? 0x9C00 : 0x9800;
    uint16_t tile_base_address = (bit(lcdc, 4) == 1)? 0x8000 : 0x9000;

    for (uint32_t x = 0; x < SCREEN_WIDTH; x++) {
        for (uint32_t y = 0; y < SCREEN_HEIGHT; y++) {
            uint32_t i = ((x + scx) % 256);
            uint32_t j = ((y + scy) % 256);

            // Read the tile index from memory
            uint16_t tile_addr;
            int16_t tile_idx = memory.load_byte(base_address + ((j/8) * TILES_PER_ROW) + (i/8));

            tile_addr = tile_base_address + tile_idx * TILE_SIZE;

            uint8_t lower = memory.load_byte(tile_addr + ((j % 8) * 2));
            uint8_t upper = memory.load_byte(tile_addr + ((j % 8) * 2) + 1);

            uint8_t palette_idx = (bit(upper, 8 - (i % 8)) << 1) | bit(lower, 8 - (i % 8));

            set_pixel(x, y, palette[palette_idx & 0b00000011]);
        }
    }
}

void GPU::load_window_into_buffer() {
    // TODO Debug this
    uint8_t lcdc = memory.load_byte(LCDC);

    if (!bit(lcdc, 5)) {
        return;
    }

    // Draw the window
    uint8_t wx = memory.load_byte(WX);
    uint8_t wy = memory.load_byte(WY);

    uint16_t base_address = (bit(lcdc, 6) == 1)? 0x9C00 : 0x9800;
    uint16_t tile_base_address = (bit(lcdc, 4) == 1)? 0x8000 : 0x9000;

    for (uint32_t x = 0; x < BGR_WIDTH; x++) {
        for (uint32_t y = 0; y < BGR_HEIGHT; y++) {

            if (x + wx >= SCREEN_WIDTH or y + wy >= SCREEN_HEIGHT) {
                continue; // This pixel is not visible.
            }

            // Read the tile index from memory
            uint16_t tile_addr;
            int16_t tile_idx = memory.load_byte(base_address + ((y/8) * TILES_PER_ROW) + (x/8));

            tile_addr = tile_base_address + tile_idx * TILE_SIZE;

            uint8_t lower = memory.load_byte(tile_addr + ((y % 8) * 2));
            uint8_t upper = memory.load_byte(tile_addr + ((y % 8) * 2) + 1);

            uint8_t palette_idx = (bit(upper, 8 - (x % 8)) << 1) | bit(lower, 8 - (x % 8));

            set_pixel(x + wx, y + wy, palette[palette_idx & 0b00000011]);
        }
    }
}

void GPU::draw_buffer() {
    uint8_t lcdc = memory.load_byte(LCDC);

    if (bit(lcdc, 7) == 0) {
        return;
    }

     // The 4 palette colors
    uint8_t bgp = memory.load_byte(BGP);
    for (int i = 0; i < 4; i++) {
        palette[i] = COLORS[bgp & 0b00000011];
        bgp = bgp >> 2;
    }

    load_background_into_buffer();
    load_window_into_buffer();

    glDrawPixels(SCREEN_WIDTH, SCREEN_HEIGHT, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, buffer);

    glfwSwapBuffers(window);
    glfwPollEvents();
}