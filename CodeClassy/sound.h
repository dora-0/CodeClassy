#pragma once
#pragma warning (disable:4996)
#include <fmod.h>
int fmod_init(void);
int play_BGM(const char *filename, FMOD_MODE mode);
int play_SE(const char *filename, FMOD_MODE mode);
int toggleSound(int x, int y);
int keyInput_sound(FMOD_CHANNEL *channel, int *volume, char str[][MAX_ARRAY_SIZE], int *cursor, int row, int x, int y);
int stop_BGM(void);
int stop_SE(void);
int fmod_close(void);