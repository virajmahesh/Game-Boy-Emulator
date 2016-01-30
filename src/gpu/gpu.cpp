/*
 * @author: Viraj Mahesh (virajmahesh@gmail.com)
 */

#include "gpu.h"

Pixel::Pixel() {
    b = 255;
    g = 255;
    r = 255;
    a = 0;
}

Pixel::Pixel(uint8_t b, uint8_t g, uint8_t r, uint8_t a) {
    this->b = b;
    this->g = g;
    this->r = r;
    this->a = a;
}

bool Pixel::operator==(const Pixel & p) {
    return b == p.b and g == p.g and r == p.r;
}

bool Pixel::operator!=(const Pixel & p) {
    return b != p.b or g != p.g or r != p.r;
}

GPU::GPU(Memory & mem) : memory(mem) {
    cycles = 0;
    mode = OAM_ACCESS;

    // Initialize GLFW and create a new window.
    glfwInit();
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game Boy Emulator", NULL, NULL);

    glfwSwapInterval(1);
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, window_resized);

    // Create a new buffer of pixels.
    buffer = new Pixel[SCREEN_WIDTH * SCREEN_HEIGHT];
}

void GPU::window_resized(GLFWwindow *window, int width, int height) {
    window_width =  width;
    window_height = height;
}

void GPU::render_screen(uint32_t cpu_cycles) {
    cycles += cpu_cycles;

    uint8_t ly = memory.load_byte(LY);
    uint8_t lyc = memory.load_byte(LYC);
    uint8_t stat = memory.load_byte(STAT);
    uint8_t interrupt_flag = memory.load_byte(IF);

    switch (mode) {
        case OAM_ACCESS:
            if (cycles > OAM_ACCESS_TIME) {
                mode = VRAM_ACCESS;
                cycles -= OAM_ACCESS_TIME;

                set_bit(stat, 0);
                set_bit(stat, 1);
            }
            break;

        case VRAM_ACCESS:
            if (cycles > VRAM_ACCESS_TIME) {
                mode = H_BLANK;
                cycles -= VRAM_ACCESS_TIME;

                reset_bit(stat, 0);
                reset_bit(stat, 1);
            }
            break;

        case H_BLANK:
            if (cycles >= H_BLANK_TIME) {
                ly += 1;
                if (ly == SCREEN_HEIGHT) {
                    mode = V_BLANK;

                    set_bit(stat, 0);
                    reset_bit(stat, 1);
                    set_bit(interrupt_flag, 0);

                    draw_screen();
                }
                else {
                    mode = OAM_ACCESS;
                }
                cycles -= H_BLANK_TIME;
            }
            break;

        case V_BLANK:
            if (cycles > H_BLANK_TIME) {
                ly += 1;
                if (ly == SCREEN_HEIGHT + V_BLANK_LINES) {
                    mode = OAM_ACCESS;

                    ly = 0;
                    set_bit(stat, 1);
                    reset_bit(stat, 0);
                }
                cycles -= H_BLANK_TIME;
            }
            break;
    }

    if (lyc == ly) {
        if (get_bit(stat, 6)) {
            set_bit(interrupt_flag, 1);
        }
        set_bit(stat, 2);
    }
    else {
        reset_bit(stat, 2);
    }
    if (get_bit(stat, 5) and mode == OAM_ACCESS) {
        set_bit(interrupt_flag, 1);
    }
    if (get_bit(stat, 4) and mode == V_BLANK) {
        set_bit(interrupt_flag, 1);
    }
    if (get_bit(stat, 3) and mode == H_BLANK) {
        set_bit(interrupt_flag, 1);
    }

    memory.store_byte(LY, ly);
    memory.store_byte(STAT, stat);
    memory.store_byte(IF, interrupt_flag);
}

Pixel GPU::get_pixel(int x, int y) {
    y = SCREEN_HEIGHT - y - 1;
    return *(buffer + x + (y * SCREEN_WIDTH));
}

void GPU::set_pixel(int x, int y, Pixel & pixel) {
    // Find the right pixel.
    y = SCREEN_HEIGHT - y - 1;
    Pixel *p = buffer + x + (y * SCREEN_WIDTH);

    // Set the pixel value.
    p->r = pixel.r;
    p->g = pixel.g;
    p->b = pixel.b;
    p->a = pixel.a;
}

void GPU::load_background_into_buffer() {
    uint8_t lcdc = memory.load_byte(LCDC);

    // Check if background is enabled.
    if (!get_bit(lcdc, 0)) {
        return;
    }

    uint8_t scx = memory.load_byte(SCX);
    uint8_t scy = memory.load_byte(SCY);

    uint16_t base_address = base_addresses[get_bit(lcdc, 3)];
    uint16_t tile_base_address = tile_addresses[get_bit(lcdc, 4)];

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            int i = (x + scx) % BGR_WIDTH;
            int j = (y + scy) % BGR_HEIGHT;

            // Map pixel (i, j) to tile (m, n).
            int m = i/ROWS_PER_TILE;
            int n = j/COLUMNS_PER_TILE;

            // Read the tile index from memory.
            uint16_t tile_addr;
            // Check if the tile index is signed or unsigned.
            if (get_bit(lcdc, 4) == 1) {
                uint8_t tile_idx = memory.load_byte(base_address + (n * TILES_PER_ROW) + m);
                tile_addr = tile_base_address + tile_idx * TILE_SIZE;
            }
            else {
                int8_t tile_idx = memory.load_byte(base_address + (n * TILES_PER_ROW) + m);
                tile_addr = tile_base_address + tile_idx * TILE_SIZE;
            }

            uint8_t tile_y = j % COLUMNS_PER_TILE;
            uint8_t tile_x = ROWS_PER_TILE - (i % ROWS_PER_TILE) - 1;

            uint8_t lower = memory.load_byte(tile_addr + (tile_y * 2));
            uint8_t upper = memory.load_byte(tile_addr + (tile_y * 2) + 1);

            uint8_t palette_idx = (get_bit(upper, tile_x) << 1) | get_bit(lower, tile_x);

            set_pixel(x, y, palette[palette_idx]);
        }
    }
}

void GPU::load_window_into_buffer() {

    uint8_t lcdc = memory.load_byte(LCDC);

    if (get_bit(lcdc, 5) == 0) {
        return;
    }

    // Draw the window.
    uint8_t wx = memory.load_byte(WX) - 7;
    uint8_t wy = memory.load_byte(WY);

    uint16_t base_address = base_addresses[get_bit(lcdc, 6)];
    uint16_t tile_base_address = tile_addresses[get_bit(lcdc, 4)];

    for (int x = 0; x < BGR_WIDTH; x++) {
        for (int y = 0; y < BGR_HEIGHT; y++) {

            if (x + wx >= SCREEN_WIDTH or y + wy >= SCREEN_HEIGHT) {
                continue; // This pixel is not visible.
            }

            int m = x/8;
            int n = y/8;

            // Read the tile index from memory
            uint16_t tile_addr;
            if (get_bit(lcdc, 4) == 1) {
                uint8_t tile_idx = memory.load_byte(base_address + (n * TILES_PER_ROW) + m);
                tile_addr = tile_base_address + tile_idx * TILE_SIZE;
            }
            else {
                int8_t tile_idx = memory.load_byte(base_address + (n * TILES_PER_ROW) + m);
                tile_addr = tile_base_address + tile_idx * TILE_SIZE;
            }

            uint8_t tile_y = y % COLUMNS_PER_TILE;
            uint8_t tile_x = ROWS_PER_TILE - (x % ROWS_PER_TILE) - 1;

            uint8_t lower = memory.load_byte(tile_addr + (tile_y * 2));
            uint8_t upper = memory.load_byte(tile_addr + (tile_y * 2) + 1);

            uint8_t palette_idx = (get_bit(upper, tile_x) << 1) | get_bit(lower, tile_x);

            set_pixel(x + wx, y + wy, palette[palette_idx]);
        }
    }
}

void GPU::load_sprites_into_buffer() {
    // Parse the sprite palettes.
    Pixel palettes[2][4];
    uint8_t palette_0 = memory.load_byte(OBP0);
    uint8_t palette_1 = memory.load_byte(OBP1);

    for (int i = 0; i < 4; i++) {
        palettes[0][i] = colors[palette_0 & 0b00000011];
        palettes[1][i] = colors[palette_1 & 0b00000011];
        palette_0 = palette_0 >> 2;
        palette_1 = palette_1 >> 2;
    }

    uint16_t base_address = 0xFE00;
    uint16_t tile_base_address = 0x8000;

    Sprite s;
    for (int i = 0; i < NUM_SPRITES; i++) {
        memory.copy(&s, base_address + i * SPRITE_SIZE, SPRITE_SIZE);
        // For each row.
        for (int n = 0; n < ROWS_PER_TILE; n++) {
            uint16_t tile_address = tile_base_address + s.tile_idx * TILE_SIZE;
            uint8_t lower = memory.load_byte(tile_address + n * TILE_ROW_SIZE);
            uint8_t upper = memory.load_byte(tile_address + n * TILE_ROW_SIZE + 1);

            // For each pixel.
            for (int m = 0; m < COLUMNS_PER_TILE; m++) {
                int x = s.x - SPRITE_X_OFFSET + m;
                int y = s.y - SPRITE_Y_OFFSET + n;

                // Check if the pixel is on the screen.
                if (x < 0 or x >= SCREEN_WIDTH) {
                    continue;
                }
                if (y < 0 or y >= SCREEN_HEIGHT) {
                    continue;
                }

                int palette_idx = (get_bit(upper, 7 - m) << 1) | get_bit(lower, 7 - m);
                Pixel sprite_pixel = palettes[s.palette][palette_idx];

                // Check if the pixel is transparent.
                if (sprite_pixel == WHITE) {
                    continue;
                }

                // Sprite is behind the background.
                if (s.sprite_priority == 1) {
                    if (get_pixel(x, y) != WHITE) {
                        continue;
                    }
                }
                set_pixel(x, y, sprite_pixel);
            }
        }
    }
}

void GPU::set_zoom_factor() {
    float xfactor = (float)window_width/SCREEN_WIDTH;
    float yfactor = (float)window_height/SCREEN_HEIGHT;

    glPixelZoom(xfactor, yfactor);
}

void GPU::draw_screen() {
    uint8_t lcdc = memory.load_byte(LCDC);

    if (get_bit(lcdc, 7) == 0) {
        return;
    }

    // Parse the background and window palette.
    uint8_t bgp = memory.load_byte(BGP);
    for (int i = 0; i < 4; i++) {
        palette[i] = colors[bgp & 0b00000011];
        bgp = bgp >> 2;
    }

    load_background_into_buffer();
    load_window_into_buffer();
    load_sprites_into_buffer();

    set_zoom_factor();

    glDrawPixels(SCREEN_WIDTH, SCREEN_HEIGHT, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, buffer);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

GLFWwindow* GPU::get_window() {
    return window;
}