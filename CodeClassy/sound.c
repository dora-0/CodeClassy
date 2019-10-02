#include <stdio.h>
#include "main.h"
#include "sound.h"
/*
	** sound.c : FMOD ���̺귯���� ����� ���� ����

	1) static FMOD_SYSTEM *g_System
	** FMOD_SYSTEM���� ���ǵ� ����ü

	2) static FMOD_SOUND *BGM
	** ���� ���� (���� �Է�)

	3) static FMOD_SOUND *SE
	** ���� ���� (���� �Է�)

	4) static FMOD_CHANNEL *CH1
	** ��� ä�� 1 (BGM)

	5) static FMOD_CHANNEL *CH2
	** ��� ä�� 2 (SE)

	6) int bgm_volume (��� �� float)
	** BGM �ҷ� ���� ����

	7) int se_volume (��� �� float)
	** SE �ҷ� ���� ����

	8) int fmod_init(void)
	** FMOD �ý��� �ʱ�ȭ �Լ�

	FMOD �ý��� ���� �� �ʱ�ȭ

	==

	9) int play_BGM(const char *filename, FMOD_MODE mode)
	10) int play_SE(const char *filename, FMOD_MODE mode)
	** BGM, SE ��� �Լ�

	���� �̸��� ���� ��带 �Է¹��� �� ���

	==

	11) int toggleSound(int x, int y)
	12) int keyInput_sound(FMOD_CHANNEL *channel, int *volume, char str[][MAX_ARRAY_SIZE], int *cursor, int row, int x, int y)
	** �ɼ��� ���� ���� �޴��� �����ϱ� ���� ���۵� Ư�� �Լ�

	==

	13) int stop_BGM(void)
	14) int stop_SE(void)

	�� ä�ο� ����Ǵ� ���� ����

	==

	15) int fmod_close(void)

	FMOD �ý��� ����

*/

static FMOD_SYSTEM *g_System;
static FMOD_SOUND *BGM;
static FMOD_SOUND *SE;
static FMOD_CHANNEL *CH1 = NULL;
static FMOD_CHANNEL *CH2 = NULL;
static int bgm_volume = 7;
static int se_volume = 10;

int fmod_init(void) {
	FMOD_System_Create(&g_System);
	FMOD_System_Init(g_System, 32, FMOD_INIT_NORMAL, NULL);

	return 0;
}

int play_BGM(const char *filename, FMOD_MODE mode) {
	stop_BGM();
	FMOD_System_CreateSound(g_System, filename, mode, 0, &BGM);

	FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, BGM, 0, &CH1);
	FMOD_Channel_SetVolume(CH1, (float)bgm_volume / 10);

	FMOD_System_Update(g_System);
	return 0;
}

int play_SE(const char *filename, FMOD_MODE mode) {
	FMOD_System_CreateSound(g_System, filename, mode, 0, &SE);

	FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, SE, 0, &CH2);
	FMOD_Channel_SetVolume(CH2, (float)se_volume / 10);

	FMOD_System_Update(g_System);
	return 0;
}

int toggleSound(int x, int y) {
	int i, j, V1 = 15, V2 = 0;
	int cursor = STR1;
	int ref_speed = get_ref_speed();

	char str[][MAX_ARRAY_SIZE] = {
		"  BGM: �� 0 ��  ",
		"   SE: �� 0 ��  ",
		"   -- Exit --"
	};
	int row = sizeof(str) / sizeof(str[0]); //�迭�� ���� ũ��

	sprintf(str[0], "  BGM: �� %d ��  ", bgm_volume);
	sprintf(str[1], "   SE: �� %d ��  ", se_volume);
	
	for (i = y; i < y + row; i++) {
		for (j = x; j < x + 20; j++) {
			ScreenPrint(j, i, " ");
		}
	}

	//'z'Ű�� ������ �ݺ��� ����
	while (keyInput(&cursor, 0, row - 1)) {
		for (i = 0; i < row; i++) {
			//i = 0���� row - 1���� �ݺ�
			
			V1 = (cursor == i) ? 12 : 15;
			setColor(V1, V2);
			//LPRINT(x, y + i, str[i]); //�迭 �׸���
			ScreenPrint(x, y + i, str[i]);
		}
		//�ӵ� ����
		Sleep(ref_speed);
	}

	switch (cursor) {
	case STR1:
		while (keyInput_sound(CH1, &bgm_volume, str, &cursor, row, x, y));
		break;
	case STR2:
		while (keyInput_sound(CH2, &se_volume, str, &cursor, row, x, y));
		break;
	case STR3:
		return 0;
		break;
	default:
		break;
	}

	return 1;
}

int keyInput_sound(FMOD_CHANNEL *channel, int *volume, char str[][MAX_ARRAY_SIZE], int *cursor, int row, int x, int y) {
	int key = 0, i = 0;
	int V1 = 15, V2 = 0;
	setColor(V1, V2);

	if (_kbhit()) {
		key = _getch();

		//UP DOWN LEFT RIGHT
		if (key == EXTEND_KEY) {
			key = _getch();

			switch (key) {
			case KEY_LEFT:
			case KEY_UP:
				if (*volume > 0)
					(*volume)--;
				else
					*volume = 0;
				break;
			case KEY_RIGHT:
			case KEY_DOWN:
				if (*volume < 10)
					(*volume)++;
				else
					*volume = 10;
				break;
			}
		}

		else if (key == KEY_Z) {
			return 0;
		}
	}

	FMOD_Channel_SetVolume(channel, (float)*volume / 10);
	sprintf(str[0], "  BGM: �� %d ��  ", bgm_volume);
	sprintf(str[1], "   SE: �� %d ��  ", se_volume);

	for (i = 0; i < row; i++) {
		//i = 0���� row - 1���� �ݺ�
		//Ŀ�� ��ġ�� ���ٸ� �ؽ�Ʈ ������ ���������� ����, �ƴϸ� �������
		V1 = (*cursor == i) ? 14 : 15;
		setColor(V1, V2);
		//LPRINT(x, y + i, str[i]); //�迭 �׸���
		ScreenPrint(x, y + i, str[i]);
	}
	return 1;
}

int stop_BGM(void) {
	FMOD_Channel_Stop(CH1);
	FMOD_Sound_Release(BGM);
	return 0;
}

int stop_SE(void) {
	FMOD_Channel_Stop(CH2);
	FMOD_Sound_Release(SE);
	return 0;
}

int fmod_close(void) {
	FMOD_System_Close(g_System);
	FMOD_System_Release(g_System);
	return 0;
}