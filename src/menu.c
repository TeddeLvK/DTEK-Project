#include "menu.h"
#include <stdint.h>

#define UART_REG (*(volatile uint32_t *)0x40000004)
#define BUTTON_REG (*(volatile uint32_t *)0x40000000)
#define BTN_START (1 << 6)

void uart_print(const char *str) {
    while (*str) {
        UART_REG = *str++;
    }
}

void show_start_menu(void) {
    uart_print("\033[2J\033[H");
    uart_print("TETRIS DTEK-V\r\n");
    uart_print("Press START button to begin...\r\n");
    
    while (!(BUTTON_REG & BTN_START)) {
        __asm__ volatile("nop");
    }
}