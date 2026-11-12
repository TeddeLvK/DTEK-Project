#include <stdint.h>
#include "tetris.h"
#include "display.h"
#include "menu.h"

#define BUTTON_REG (*(volatile uint32_t *)0x40000000)

#define BTN_LEFT   (1 << 0)
#define BTN_RIGHT  (1 << 1)
#define BTN_ROTATE (1 << 2)
#define BTN_DOWN   (1 << 3)
#define BTN_DROP   (1 << 4)
#define BTN_HOLD   (1 << 5)
#define BTN_START  (1 << 6)

long long get_time_ms(void) {
    uint32_t cycles_low, cycles_high;
    __asm__ volatile (
        "rdcycle %0\n\t"
        "rdcycleh %1\n\t"
        : "=r" (cycles_low), "=r" (cycles_high)
    );
    uint64_t cycles = ((uint64_t)cycles_high << 32) | cycles_low;
    return (long long)(cycles / 50000);
}

int main(void) {
    display_init();
    tetris_init();
    
    show_start_menu();
    
    long long last_drop = get_time_ms();
    long long lock_timer = 0; 
    int current_lock_limit = 500; 
    long long now;
    uint32_t last_btns = 0;

    while(1) {
        uint32_t btns = BUTTON_REG;
        uint32_t btns_pressed = btns & ~last_btns;
        last_btns = btns;
        
        int dx = 0, dy = 0, rot = 0, hard_drop = 0, hold = 0, soft_drop = 0;

        if (game_over) {
            if (btns_pressed & BTN_START) {
                tetris_init();
                last_drop = get_time_ms();
                lock_timer = 0;
                current_lock_limit = 500;
            }
            display_clear();
            tetris_draw();
            display_update();
            continue;
        }

        if (btns_pressed & BTN_LEFT) dx = -1;
        if (btns_pressed & BTN_RIGHT) dx = 1;
        if (btns_pressed & BTN_ROTATE) rot = 1;
        if (btns & BTN_DOWN) { dy = 1; soft_drop = 1; }
        if (btns_pressed & BTN_DROP) hard_drop = 1;
        if (btns_pressed & BTN_HOLD) hold = 1;

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

        int delay = 20000;
        while(delay--) __asm__ volatile("nop");
    }
    return 0;
}