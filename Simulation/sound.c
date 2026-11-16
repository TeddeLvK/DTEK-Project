#include "sound.h"

#ifdef _WIN32
#include <windows.h>
/* On Windows, we use the Multimedia API (requires linking -lwinmm) */

static int is_muted = 0;

void sound_stop(void) {
    mciSendString("stop soundtrack", NULL, 0, NULL);
    mciSendString("close soundtrack", NULL, 0, NULL);
}

void sound_start(void) {
    sound_stop();
    if (is_muted) return;
    mciSendString("open \"../Assets/TetrisSoundTrack.mp3\" type mpegvideo alias soundtrack", NULL, 0, NULL);
    mciSendString("play soundtrack repeat", NULL, 0, NULL);
}

void sound_pause(void) {
    sound_stop();
}

void sound_resume(void) {
    if (is_muted) return;
    sound_start();
}

void sound_toggle_mute(void) {
    is_muted = !is_muted;
    if (is_muted) {
        sound_stop();
    } else {
        sound_start();
    }
}

int sound_is_muted(void) {
    return is_muted;
}

void sound_play_block_placed(void) {
    if (is_muted) return;
    mciSendString("close block", NULL, 0, NULL);
    mciSendString("open \"../Assets/BlockPlaced.mp3\" type mpegvideo alias block", NULL, 0, NULL);
    mciSendString("play block", NULL, 0, NULL);
}

void sound_play_clear_line(void) {
    if (is_muted) return;
    mciSendString("close clear", NULL, 0, NULL);
    mciSendString("open \"../Assets/ClearLine.mp3\" type mpegvideo alias clear", NULL, 0, NULL);
    mciSendString("play clear", NULL, 0, NULL);
}

void sound_play_game_over(void) {
    if (is_muted) return;
    mciSendString("close gameover", NULL, 0, NULL);
    mciSendString("open \"../Assets/GameOver.mp3\" type mpegvideo alias gameover", NULL, 0, NULL);
    mciSendString("play gameover", NULL, 0, NULL);
}

#else
/* MacOS & Linux Implementation */
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

static pid_t audio_pid = -1;
static int is_muted = 0;

void sound_stop(void) {
    if (audio_pid > 0) {
        kill(-audio_pid, SIGKILL);
        waitpid(audio_pid, NULL, 0);
        audio_pid = -1;
    }
}

void sound_start(void) {
    sound_stop();
    if (is_muted) return;
    
    audio_pid = fork();
    if (audio_pid == 0) {
        setpgid(0, 0);
        
        while (1) {
            pid_t play_pid = fork();
            if (play_pid == 0) {
#ifdef __linux__
                execlp("ffplay", "ffplay", "-nodisp", "-autoexit", "-loglevel", "quiet", "../Assets/TetrisSoundTrack.mp3", NULL);
#else
                execlp("afplay", "afplay", "../Assets/TetrisSoundTrack.mp3", NULL);
#endif
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
    sound_stop();
}

void sound_resume(void) {
    if (is_muted) return;
    sound_start();
}

void sound_toggle_mute(void) {
    is_muted = !is_muted;
    if (is_muted) {
        sound_stop();
    } else {
        sound_start();
    }
}

int sound_is_muted(void) {
    return is_muted;
}

void sound_play_block_placed(void) {
    if (is_muted) return;
    pid_t pid = fork();
    if (pid == 0) {
#ifdef __linux__
        execlp("ffplay", "ffplay", "-nodisp", "-autoexit", "-loglevel", "quiet", "../Assets/BlockPlaced.mp3", NULL);
#else
        execlp("afplay", "afplay", "../Assets/BlockPlaced.mp3", NULL);
#endif
        _exit(1);
    }
}

void sound_play_clear_line(void) {
    if (is_muted) return;
    pid_t pid = fork();
    if (pid == 0) {
#ifdef __linux__
        execlp("ffplay", "ffplay", "-nodisp", "-autoexit", "-loglevel", "quiet", "../Assets/ClearLine.mp3", NULL);
#else
        execlp("afplay", "afplay", "../Assets/ClearLine.mp3", NULL);
#endif
        _exit(1);
    }
}

void sound_play_game_over(void) {
    if (is_muted) return;
    pid_t pid = fork();
    if (pid == 0) {
#ifdef __linux__
        execlp("ffplay", "ffplay", "-nodisp", "-autoexit", "-loglevel", "quiet", "../Assets/GameOver.mp3", NULL);
#else
        execlp("afplay", "afplay", "../Assets/GameOver.mp3", NULL);
#endif
        _exit(1);
    }
}
#endif