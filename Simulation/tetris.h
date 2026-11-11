#ifndef TETRIS_H
#define TETRIS_H

#include <stdint.h>

#define BOARD_W 10
#define BOARD_H 20

extern uint8_t board[BOARD_H][BOARD_W];
extern int current_x, current_y;
extern int current_piece;
extern int current_rotation;
extern int held_piece;
extern int next_piece;
extern const uint16_t pieces[7][4];

/* Upgraded to unsigned long long to support scores up to 18 quintillion */
extern unsigned long long score;
extern unsigned long long high_score;
extern int level;
extern int total_lines;
extern int game_over;

void tetris_init(void);
void tetris_update(int dx, int dy, int rotate, int hard_drop, int hold, int soft_drop);
void tetris_draw(void);

int check_collision(int px, int py, int r);

#endif