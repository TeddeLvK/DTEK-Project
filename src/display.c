#include "display.h"

#define SPI_BASE 0x40001000
#define SPI_DATA (*(volatile uint32_t*)(SPI_BASE))
#define SPI_CTRL (*(volatile uint32_t*)(SPI_BASE + 4))

uint8_t frame_buffer[512];

void display_init(void) {
    SPI_CTRL = 0x01;
}

void display_clear(void) {
    int i;
    for(i = 0; i < 512; i++) {
        frame_buffer[i] = 0;
    }
}

void display_update(void) {
    int i;
    for(i = 0; i < 512; i++) {
        while(!(SPI_CTRL & 0x02));
        SPI_DATA = frame_buffer[i];
    }
}

void display_draw_pixel(int x, int y, int color) {
    if(x < 0 || x >= 128 || y < 0 || y >= 32) return;
    int page = y / 8;
    int bit = y % 8;
    int index = x + (page * 128);
    
    if(color) {
        frame_buffer[index] |= (1 << bit);
    } else {
        frame_buffer[index] &= ~(1 << bit);
    }
}