#ifndef TETRIS_H
#define TETRIS_H

#include <stdint.h>

#define BOARD_W 10
#define BOARD_H 20

extern uint8_t board[BOARD_H][BOARD_W];
extern int current_x, current_y;
extern int current_piece;
extern int current_rotation;

void tetris_init(void);
void tetris_update(int dx, int dy, int rotate);
void tetris_draw(void);

#endif