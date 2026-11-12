#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

void display_init(void);
void display_clear(void);
void display_draw_pixel(int x, int y, int color);
void display_update(void);

#endif