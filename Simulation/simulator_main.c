#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdint.h>
#include "tetris.h"

struct termios orig_termios;
uint8_t sim_display[BOARD_H][BOARD_W * 2];

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf("\e[?25h");
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    printf("\e[?25l");
}

void display_init(void) {
    enable_raw_mode();
}

void display_clear(void) {
    for(int y = 0; y < BOARD_H; y++) {
        for(int x = 0; x < BOARD_W * 2; x++) {
            sim_display[y][x] = 0;
        }
    }
}

void display_draw_pixel(int x, int y, int color) {
    if(x < 0 || x >= BOARD_W * 2 || y < 0 || y >= BOARD_H) return;
    sim_display[y][x] = color;
}

void display_update(void) {
    printf("\e[H");
    for(int y = 0; y < BOARD_H; y++) {
        printf("|");
        for(int x = 0; x < BOARD_W; x++) {
            if(sim_display[y][x * 2]) {
                printf("[]");
            } else {
                printf("  ");
            }
        }
        printf("|\r\n");
    }
    printf("----------------------\r\n");
    fflush(stdout);
}

long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

int main() {
    display_init();
    tetris_init();

    long long last_drop = get_time_ms();
    long long now;

    printf("\e[2J\e[H");

    while(1) {
        int dx = 0, dy = 0, rot = 0;
        char c;
        
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == 'q') break;
            if (c == 'a') dx = -1;
            if (c == 'd') dx = 1;
            if (c == 'w') rot = 1;
            if (c == 's') dy = 1;
        }

        now = get_time_ms();
        if (now - last_drop > 500) {
            dy = 1;
            last_drop = now;
        }

        if (dx != 0 || dy != 0 || rot != 0) {
            tetris_update(dx, dy, rot);
        }

        display_clear();
        tetris_draw();
        display_update();

        usleep(16000);
    }
    return 0;
}