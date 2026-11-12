#include <stdio.h>
#include <unistd.h>
#include "menu.h"

void show_start_menu(void)
{
    int bounce_y = 0;
    int bounce_dir = 1;
    int frame_counter = 0;
    char c;
    int max_bounce = 6;

    printf("\e[2J\e[H");

    while (1)
    {
        if (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (c == 's' || c == 'S')
            {
                break;
            }
            if (c == 'q' || c == 'Q')
            {
                _exit(0);
            }
        }

        printf("\e[H");

        printf("      _____  ___  _____  ___  ___  ___ \r\n");
        printf("     |_   _|| __||_   _|| _ \\|_ _|/ __|\r\n");
        printf("       | |  | _|   | |  |   / | | \\__ \\\r\n");
        printf("       |_|  |___|  |_|  |_|_\\|___||___/\r\n\r\n\r\n");
        for (int i = 0; i < bounce_y; i++)
        {
            printf("\e[K\r\n");
        }

        printf(" WELCOME TO THEODOR'S AND JENS'S DTEK PROJEKT\e[K\r\n");

        for (int i = bounce_y; i < max_bounce; i++)
        {
            printf("\e[K\r\n");
        }

        printf("\e[K\r\n\e[K\r\n");
        printf("         PRESS 'S' TO START PLAYING\e[K\r\n");

        for (int i = 0; i < 10; i++)
        {
            printf("\e[K\r\n");
        }

        fflush(stdout);

        frame_counter++;
        if (frame_counter >= 18)
        {
            bounce_y += bounce_dir;
            if (bounce_y >= max_bounce || bounce_y <= 0)
            {
                bounce_dir = -bounce_dir;
            }
            frame_counter = 0;
        }

        usleep(16000);
    }

    printf("\e[2J\e[H");
    fflush(stdout);
}