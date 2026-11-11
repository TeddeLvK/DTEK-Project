#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdint.h>
#include "tetris.h"
#include "menu.h"

struct termios orig_termios;
uint8_t sim_display[BOARD_H][BOARD_W * 2];

int is_paused = 0;

void save_highscore() {
    FILE *f = fopen("highscore.txt", "w");
    if (f) {
        fprintf(f, "%llu\n", high_score);
        fclose(f);
    }
}

void load_highscore() {
    FILE *f = fopen("highscore.txt", "r");
    if (f) {
        if (fscanf(f, "%llu", &high_score) != 1) {
            high_score = 0;
        }
        
        if (high_score > 9999999999999ULL) {
            high_score = 0;
        }
        
        fclose(f);
    }
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf("\e[?25h");
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    atexit(save_highscore);
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
    
    printf("  _____  ___  _____  ___  ___  ___ \r\n");
    printf(" |_   _|| __||_   _|| _ \\|_ _|/ __|\r\n");
    printf("   | |  | _|   | |  |   / | | \\__ \\\r\n");
    printf("   |_|  |___|  |_|  |_|_\\|___||___/\r\n\r\n\r\n");

    for(int y = 0; y < BOARD_H; y++) {
        printf("|");
        
        if (game_over && y == 7) {
            printf("     GAME OVER      ");
        } else if (is_paused && y == 7) {
            printf("       PAUSED       ");
        } else if (game_over && y == 9) {
            printf("Score: %-13llu", score);
        } else if (game_over && y == 10) {
            printf("High:  %-13llu", high_score);
        } else if (game_over && y == 12) {
            printf(" Press R to Restart ");
        } else if (game_over && y == 13) {
            printf("  Press Q to Quit   ");
        } else {
            for(int x = 0; x < BOARD_W; x++) {
                if(sim_display[y][x * 2]) {
                    printf("[]");
                } else {
                    printf("  ");
                }
            }
        }
        
        if (y == 2) {
            printf("|  Score: %-12llu\e[K\r\n", score);
        } else if (y == 3) {
            printf("|  High : %-12llu\e[K\r\n", high_score);
        } else if (y == 4) {
            printf("|  Level: %-6d\e[K\r\n", level);
        } else if (y == 5) {
            printf("|  Lines: %-6d\e[K\r\n", total_lines);
        } else {
            printf("|\e[K\r\n");
        }
    }
    
    printf("----------------------\e[K\r\n");
    printf("Hold:       Next:\e[K\r\n");
    
    for (int y = 0; y < 4; y++) {
        if (held_piece != -1) {
            uint16_t h_shape = pieces[held_piece][0];
            for (int x = 0; x < 4; x++) {
                if (h_shape & (1 << (15 - (y * 4 + x)))) printf("[]");
                else printf("  ");
            }
        } else {
            if (y == 1) printf("[Empty] ");
            else printf("        ");
        }
        
        printf("    ");
        
        uint16_t n_shape = pieces[next_piece][0];
        for (int x = 0; x < 4; x++) {
            if (n_shape & (1 << (15 - (y * 4 + x)))) printf("[]");
            else printf("  ");
        }
        printf("\e[K\r\n");
    }
    fflush(stdout);
}

long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

int main() {
    load_highscore();
    display_init();
    tetris_init();
    
    show_start_menu();

    long long last_drop = get_time_ms();
    long long lock_timer = 0; 
    int current_lock_limit = 500; 
    long long now;

    printf("\e[2J\e[H");

    while(1) {
        char c;
        int dx = 0, dy = 0, rot = 0, hard_drop = 0, hold = 0, soft_drop = 0;
        
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == 'q' || c == 'Q') break;
            
            if (game_over) {
                if (c == 'r' || c == 'R') {
                    tetris_init();
                    last_drop = get_time_ms();
                    lock_timer = 0;
                    current_lock_limit = 500;
                    is_paused = 0;
                }
            } else {
                if (c == 'p' || c == 'P') {
                    is_paused = !is_paused;
                    if (!is_paused) {
                        last_drop = get_time_ms();
                        if (lock_timer != 0) {
                            lock_timer = get_time_ms();
                        }
                    }
                }
                
                if (!is_paused) {
                    if (c == ' ') hard_drop = 1;
                    if (c == 'z' || c == 'Z') hold = 1;
                    
                    if (c == '\033') {
                        char seq[2];
                        if (read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1) {
                            if (seq[0] == '[') {
                                if (seq[1] == 'A') rot = 1;
                                if (seq[1] == 'B') { 
                                    if (!check_collision(current_x, current_y + 1, current_rotation)) {
                                        dy = 1; soft_drop = 1; 
                                    }
                                }
                                if (seq[1] == 'C') dx = 1;
                                if (seq[1] == 'D') dx = -1;
                            }
                        }
                    }
                }
            }
        }

        if (game_over || is_paused) {
            display_clear();
            tetris_draw();
            display_update();
            usleep(16000);
            continue;
        }

        int drop_delay = 550 - (level * 35);
        if (drop_delay < 60) drop_delay = 60;

        now = get_time_ms();
        
        int is_touching_bottom = check_collision(current_x, current_y + 1, current_rotation);

        if (is_touching_bottom) {
            if (lock_timer == 0) {
                lock_timer = now;
            } else if (now - lock_timer > current_lock_limit) { 
                dy = 1; 
                lock_timer = 0;
            }
        } else {
            lock_timer = 0;
            current_lock_limit = 500;
            if (now - last_drop > drop_delay) {
                dy = 1;
                last_drop = now;
            }
        }

        if (dx != 0 || dy != 0 || rot != 0 || hard_drop != 0 || hold != 0) {
            int old_x = current_x;
            int old_rot = current_rotation;
            int old_piece = current_piece; 
            
            tetris_update(dx, dy, rot, hard_drop, hold, soft_drop);
            
            if (hard_drop || current_piece != old_piece) {
                last_drop = now;
                lock_timer = 0;
                current_lock_limit = 500;
            } else if (dy == 1 && !is_touching_bottom) {
                last_drop = now;
            }

            if (is_touching_bottom && lock_timer != 0 && current_piece == old_piece) {
                if (current_x != old_x || current_rotation != old_rot) {
                    lock_timer = now;
                    current_lock_limit -= 50;
                    if (current_lock_limit < 0) {
                        current_lock_limit = 0;
                    }
                }
            }
        }

        display_clear();
        tetris_draw();
        display_update();

        usleep(16000);
    }
    return 0;
}