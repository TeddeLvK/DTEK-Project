#include "tetris.h"

extern void display_draw_pixel(int x, int y, int color);

uint8_t board[BOARD_H][BOARD_W];
int current_x = 4, current_y = 0;
int current_piece = 0;
int current_rotation = 0;
int held_piece = -1;
int next_piece = 1;
int can_hold = 1;
int game_over = 0;

unsigned long long score = 0;
unsigned long long high_score = 0;
int level = 0;
int total_lines = 0;

const uint16_t pieces[7][4] = {
    {0x0F00, 0x2222, 0x00F0, 0x4444},
    {0x0660, 0x0660, 0x0660, 0x0660},
    {0x4E00, 0x4640, 0x0E40, 0x4C40},
    {0x8E00, 0x6440, 0x0E20, 0x44C0},
    {0x2E00, 0x4460, 0x0E80, 0xC440},
    {0x6C00, 0x4620, 0x06C0, 0x8C40},
    {0xC600, 0x2640, 0x0C60, 0x4C80}
};

void tetris_init(void) {
    int x, y;
    for(y = 0; y < BOARD_H; y++) {
        for(x = 0; x < BOARD_W; x++) {
            board[y][x] = 0;
        }
    }
    
    current_x = 4;
    current_y = 0;
    current_piece = 0;
    current_rotation = 0;
    next_piece = 1;
    
    held_piece = -1;
    can_hold = 1;
    score = 0;
    level = 0;
    total_lines = 0;
    game_over = 0;
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
    int lines_cleared_now = 0;
    
    for (y = BOARD_H - 1; y >= 0; y--) {
        int is_full = 1;
        for (x = 0; x < BOARD_W; x++) {
            if (board[y][x] == 0) {
                is_full = 0;
                break;
            }
        }
        
        if (is_full) {
            lines_cleared_now++;
            for (ty = y; ty > 0; ty--) {
                for (tx = 0; tx < BOARD_W; tx++) {
                    board[ty][tx] = board[ty - 1][tx];
                }
            }
            for (tx = 0; tx < BOARD_W; tx++) {
                board[0][tx] = 0;
            }
            y++; 
        }
    }
    
    if (lines_cleared_now > 0) {
        total_lines += lines_cleared_now;
        level = total_lines / 10;
        
        switch(lines_cleared_now) {
            case 1: score += 40 * (level + 1); break;
            case 2: score += 100 * (level + 1); break;
            case 3: score += 300 * (level + 1); break;
            case 4: score += 1200 * (level + 1); break;
        }
        if (score > high_score) high_score = score;
    }
}

void tetris_update(int dx, int dy, int rotate, int hard_drop, int hold, int soft_drop) {
    if(game_over) return;

    if(hold && can_hold) {
        if(held_piece == -1) {
            held_piece = current_piece;
            current_piece = next_piece;
            next_piece = (next_piece + 1) % 7;
        } else {
            int temp = current_piece;
            current_piece = held_piece;
            held_piece = temp;
        }
        current_x = 4;
        current_y = 0;
        current_rotation = 0;
        can_hold = 0;
        
        if (check_collision(current_x, current_y, current_rotation)) {
            game_over = 1;
        }
        return;
    }

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

    if(hard_drop) {
        int cells_dropped = 0;
        while(!check_collision(current_x, current_y + 1, current_rotation)) {
            current_y++;
            cells_dropped++;
        }
        score += (2 * cells_dropped);
        if (score > high_score) high_score = score;
        
        merge_piece();
        clear_lines();
        current_y = 0;
        current_x = 4;
        current_piece = next_piece;
        next_piece = (next_piece + 1) % 7;
        current_rotation = 0;
        can_hold = 1;

        if (check_collision(current_x, current_y, current_rotation)) {
            game_over = 1;
        }
    } 
    else if(dy != 0) {
        if(!check_collision(current_x, current_y + 1, current_rotation)) {
            current_y++;
            if (soft_drop) {
                score += 1;
                if (score > high_score) high_score = score;
            }
        } else {
            merge_piece();
            clear_lines();
            current_y = 0;
            current_x = 4;
            current_piece = next_piece;
            next_piece = (next_piece + 1) % 7;
            current_rotation = 0;
            can_hold = 1;

            if (check_collision(current_x, current_y, current_rotation)) {
                game_over = 1;
            }
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
    
    if (!game_over) {
        uint16_t shape = pieces[current_piece][current_rotation];
        for(y = 0; y < 4; y++) {
            for(x = 0; x < 4; x++) {
                if(shape & (1 << (15 - (y * 4 + x)))) {
                    display_draw_pixel((current_x + x) * 2, current_y + y, 1);
                }
            }
        }
    }
}