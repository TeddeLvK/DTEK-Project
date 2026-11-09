#include "tetris.h"
#include "display.h"

#define BTN_BASE 0x40000000

int get_btns(void) {
    return (*(volatile int*)BTN_BASE) & 0xF;
}

void delay(int ticks) {
    volatile int i;
    for(i = 0; i < ticks; i++) {}
}

int main(void) {
    display_init();
    tetris_init();
    
    int counter = 0;
    
    while(1) {
        int btns = get_btns();
        int dx = 0;
        int dy = 0;
        int rot = 0;
        
        if(btns & 0x1) dx = -1;
        if(btns & 0x2) dx = 1;
        if(btns & 0x4) rot = 1;
        if(btns & 0x8) dy = 1;
        
        counter++;
        if(counter > 100000) {
            dy = 1;
            counter = 0;
        }
        
        tetris_update(dx, dy, rot);
        
        display_clear();
        tetris_draw();
        display_update();
        
        delay(10000);
    }
    return 0;
}