#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include "sound.h"

static pid_t audio_pid = -1;

void sound_stop(void) {
    if (audio_pid > 0) {
        kill(-audio_pid, SIGKILL);
        waitpid(audio_pid, NULL, 0);
        audio_pid = -1;
    }
}

void sound_start(void) {
    sound_stop();
    
    audio_pid = fork();
    if (audio_pid == 0) {
        setpgid(0, 0);
        
        while (1) {
            pid_t play_pid = fork();
            if (play_pid == 0) {
                execlp("afplay", "afplay", "../Assets/TetrisSoundTrack.mp3", NULL);
                _exit(1);
            } else if (play_pid > 0) {
                int status;
                waitpid(play_pid, &status, 0);
            } else {
                break;
            }
        }
        _exit(0);
    }
}

void sound_pause(void) {
    if (audio_pid > 0) {
        kill(-audio_pid, SIGSTOP);
    }
}

void sound_resume(void) {
    if (audio_pid > 0) {
        kill(-audio_pid, SIGCONT);
    }
}

void sound_play_block_placed(void) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("afplay", "afplay", "../Assets/BlockPlaced.mp3", NULL);
        _exit(1);
    }
}

void sound_play_clear_line(void) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("afplay", "afplay", "../Assets/ClearLine.mp3", NULL);
        _exit(1);
    }
}

void sound_play_game_over(void) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("afplay", "afplay", "../Assets/GameOver.mp3", NULL);
        _exit(1);
    }
}