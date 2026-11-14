#ifndef SOUND_H
#define SOUND_H

void sound_start(void);
void sound_stop(void);
void sound_pause(void);
void sound_resume(void);
void sound_toggle_mute(void);
int sound_is_muted(void);
void sound_play_block_placed(void);
void sound_play_clear_line(void);
void sound_play_game_over(void);

#endif