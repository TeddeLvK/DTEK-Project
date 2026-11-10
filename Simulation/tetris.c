#include "tetris.h"

extern void display_draw_pixel(int x, int y, int color);

uint8_t board[BOARD_H][BOARD_W];
int current_x = 4, current_y = 0;
int current_piece = 0;
int current_rotation = 0;

const uint16_t pieces[7][4] = {
    {0x0F00, 0x2222, 0x00F0, 0x4444},
    {0x0660, 0x0660, 0x0660, 0x0660},
    {0x0E40, 0x4C40, 0x4E00, 0x4640},
    {0x0E80, 0xC440, 0x2E00, 0x44C0},
    {0x0E20, 0x4460, 0x8E00, 0x6440},
    {0x06C0, 0x8C40, 0x06C0, 0x8C40},
    {0x0C60, 0x4C80, 0x0C60, 0x4C80}
};

void tetris_init(void) {
    int x, y;
    for(y = 0; y < BOARD_H; y++) {
        for(x = 0; x < BOARD_W; x++) {
            board[y][x] = 0;
        }
    }
}

int check_collision(int px, int py, int r) {
    int i, j;
    uint16_t shape = pieces[current_piece][r];
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            if(shape & (1 << (15 - (i * 4 + j)))) {
                int nx = px + j;
                int ny = py + i;
                if(nx < 0 || nx >= BOARD_W || ny >= BOARD_H) return 1;
                if(ny >= 0 && board[ny][nx]) return 1;
            }
        }
    }
    return 0;
}

void merge_piece(void) {
    int i, j;
    uint16_t shape = pieces[current_piece][current_rotation];
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            if(shape & (1 << (15 - (i * 4 + j)))) {
                int ny = current_y + i;
                int nx = current_x + j;
                if(ny >= 0) board[ny][nx] = 1;
            }
        }
    }
}

void clear_lines(void) {
    int y, x, ty, tx;
    for (y = BOARD_H - 1; y >= 0; y--) {
        int is_full = 1;
        for (x = 0; x < BOARD_W; x++) {
            if (board[y][x] == 0) {
                is_full = 0;
                break;
            }
        }
        
        if (is_full) {
            /* Shift all rows above down by one */
            for (ty = y; ty > 0; ty--) {
                for (tx = 0; tx < BOARD_W; tx++) {
                    board[ty][tx] = board[ty - 1][tx];
                }
            }
            /* Clear the very top row */
            for (tx = 0; tx < BOARD_W; tx++) {
                board[0][tx] = 0;
            }
            /* Check this row index again since the row that fell down might also be full */
            y++; 
        }
    }
}

void tetris_update(int dx, int dy, int rotate) {
    int next_r = current_rotation;
    if(rotate) {
        next_r = (current_rotation + 1) % 4;
        if(!check_collision(current_x, current_y, next_r)) {
            current_rotation = next_r;
        }
    }
    
    if(dx != 0) {
        if(!check_collision(current_x + dx, current_y, current_rotation)) {
            current_x += dx;
        }
    }
    
    if(dy != 0) {
        if(!check_collision(current_x, current_y + 1, current_rotation)) {
            current_y++;
        } else {
            merge_piece();
            clear_lines(); /* Added line clearing check here */
            current_y = 0;
            current_x = 4;
            current_piece = (current_piece + 1) % 7;
        }
    }
}

void tetris_draw(void) {
    int x, y;
    for(y = 0; y < BOARD_H; y++) {
        for(x = 0; x < BOARD_W; x++) {
            if(board[y][x]) {
                display_draw_pixel(x * 2, y, 1);
            }
        }
    }
    
    uint16_t shape = pieces[current_piece][current_rotation];
    for(y = 0; y < 4; y++) {
        for(x = 0; x < 4; x++) {
            if(shape & (1 << (15 - (y * 4 + x)))) {
                display_draw_pixel((current_x + x) * 2, current_y + y, 1);
            }
        }
    }
}