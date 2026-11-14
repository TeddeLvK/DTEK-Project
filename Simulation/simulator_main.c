#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdint.h>
#include "tetris.h"
#include "menu.h"
#include "sound.h"

struct termios orig_termios;
uint8_t sim_display[BOARD_H][BOARD_W * 2];

int is_paused = 0;
int game_over_sound_played = 0;

void save_highscore() {
    FILE *f = fopen("highscore.txt", "w");
    if (f) {
        fprintf(f, "%llu\n", high_score);
        fclose(f);
    }
    sound_stop();
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
    sound_stop();
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
    
    /* Added \e[K to clear line artifacts during horizontal resize */
    printf("      _____  ___  _____  ___  ___  ___ \e[K\r\n");
    printf("     |_   _|| __||_   _|| _ \\|_ _|/ __|\e[K\r\n");
    printf("       | |  | _|   | |  |   / | | \\__ \\\e[K\r\n");
    printf("       |_|  |___|  |_|  |_|_\\|___||___/\e[K\r\n");
    printf("          By Theodor LvK and Jens F\e[K\r\n");
    printf("\e[K\r\n");
    printf("\e[K\r\n");

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
                if(sim_display[y][x * 2] == 1) {
                    printf("[]");
                } else if(sim_display[y][x * 2] == 2) {
                    printf("::");
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
        } else if (y == 8) {
            printf("|  ACTION      | KEY\e[K\r\n");
        } else if (y == 9) {
            printf("|  --------------------\e[K\r\n");
        } else if (y == 10) {
            printf("|  Move Left   | ←\e[K\r\n");
        } else if (y == 11) {
            printf("|  Move Right  | →\e[K\r\n");
        } else if (y == 12) {
            printf("|  Rotate      | ↑\e[K\r\n");
        } else if (y == 13) {
            printf("|  Soft Drop   | ↓\e[K\r\n");
        } else if (y == 14) {
            printf("|  Hard Drop   | SPC\e[K\r\n");
        } else if (y == 15) {
            printf("|  Hold Piece  | C\e[K\r\n");
        } else if (y == 16) {
            printf("|  Pause       | P\e[K\r\n");
        } else if (y == 17) {
            printf("|  Mute/Unmute | M\e[K\r\n");
        } else if (y == 18) {
            printf("|  Restart     | R\e[K\r\n");
        } else if (y == 19) {
            printf("|  Quit        | Q\e[K\r\n");
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
        
        if (y == 3) {
            if (sound_is_muted()) {
                printf("           [ Audio: OFF ]");
            } else {
                printf("           [ Audio: ON  ]");
            }
        }
        
        /* Stop scrolling by dropping \r\n on the absolute last line */
        if (y < 3) {
            printf("\e[K\r\n");
        } else {
            printf("\e[K"); 
        }
    }
    
    /* Clears any leftover lines below the game if the window is resized larger */
    printf("\e[J"); 
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
    sound_start();

    long long last_drop = get_time_ms();
    long long lock_timer = 0; 
    int current_lock_limit = 500; 
    long long now;

    printf("\e[2J\e[H");

    while(1) {
        char c;
        int dx = 0, dy = 0, rot = 0, hard_drop = 0, hold = 0, soft_drop = 0;
        
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == 'q' || c == 'Q') {
                sound_stop();
                break;
            }
            if (c == 'm' || c == 'M') {
                sound_toggle_mute();
            }
            
            if (game_over) {
                if (c == 'r' || c == 'R') {
                    tetris_init();
                    last_drop = get_time_ms();
                    lock_timer = 0;
                    current_lock_limit = 500;
                    is_paused = 0;
                    game_over_sound_played = 0;
                    sound_start();
                }
            } else {
                if (c == 'p' || c == 'P') {
                    is_paused = !is_paused;
                    if (is_paused) {
                        sound_pause();
                    } else {
                        sound_resume();
                        last_drop = get_time_ms();
                        if (lock_timer != 0) {
                            lock_timer = get_time_ms();
                        }
                    }
                }
                
                if (!is_paused) {
                    if (c == ' ') hard_drop = 1;
                    if (c == 'c' || c == 'C') hold = 1;
                    
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

        if (game_over) {
            if (!game_over_sound_played) {
                sound_stop();
                sound_play_game_over();
                game_over_sound_played = 1;
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
            
            int action_performed = 0;
            if(hold && can_hold) {
                if(held_piece == -1) {
                    held_piece = current_piece;
                    current_piece = next_piece;
                    next_piece = get_random_piece();
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
                action_performed = 1;
            } else {
                if(rot) {
                    try_rotation();
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
                    int lines = clear_lines();
                    if (lines > 0) {
                        sound_play_clear_line();
                    } else {
                        sound_play_block_placed();
                    }

                    current_y = 0;
                    current_x = 4;
                    current_piece = next_piece;
                    next_piece = get_random_piece();
                    current_rotation = 0;
                    can_hold = 1;

                    if (check_collision(current_x, current_y, current_rotation)) {
                        game_over = 1;
                    }
                    action_performed = 1;
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
                        int lines = clear_lines();
                        if (lines > 0) {
                            sound_play_clear_line();
                        } else {
                            sound_play_block_placed();
                        }

                        current_y = 0;
                        current_x = 4;
                        current_piece = next_piece;
                        next_piece = get_random_piece();
                        current_rotation = 0;
                        can_hold = 1;

                        if (check_collision(current_x, current_y, current_rotation)) {
                            game_over = 1;
                        }
                        action_performed = 1;
                    }
                }
            }
            
            if (hard_drop || action_performed || current_piece != old_piece) {
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

    /* Clean exit to prevent the Zsh '%' artifact */
    printf("\e[2J\e[H"); 
    fflush(stdout);

    return 0;
}