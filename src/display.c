#include "display.h"

uint8_t display_buffer[32][128];

void display_init(void) {
}

void display_clear(void) {
    int x, y;
    for(y = 0; y < 32; y++) {
        for(x = 0; x < 128; x++) {
            display_buffer[y][x] = 0;
        }
    }
}

void display_draw_pixel(int x, int y, int color) {
    if(x < 0 || x >= 128 || y < 0 || y >= 32) return;
    display_buffer[y][x] = color;
}

void display_update(void) {
}