#include <stdio.h>
#include "main.h"
#include "sound.h"
/*
	** sound.c : FMOD 라이브러리를 사용한 사운드 구현

	1) static FMOD_SYSTEM *g_System
	** FMOD_SYSTEM으로 정의된 구조체

	2) static FMOD_SOUND *BGM
	** 사운드 생성 (파일 입력)

	3) static FMOD_SOUND *SE
	** 사운드 생성 (파일 입력)

	4) static FMOD_CHANNEL *CH1
	** 출력 채널 1 (BGM)

	5) static FMOD_CHANNEL *CH2
	** 출력 채널 2 (SE)

	6) int bgm_volume (사용 시 float)
	** BGM 불륨 전역 변수

	7) int se_volume (사용 시 float)
	** SE 불륨 전역 변수

	8) int fmod_init(void)
	** FMOD 시스템 초기화 함수

	FMOD 시스템 생성 및 초기화

	==

	9) int play_BGM(const char *filename, FMOD_MODE mode)
	10) int play_SE(const char *filename, FMOD_MODE mode)
	** BGM, SE 재생 함수

	파일 이름과 실행 모드를 입력받은 후 재생

	==

	11) int toggleSound(int x, int y)
	12) int keyInput_sound(FMOD_CHANNEL *channel, int *volume, char str[][MAX_ARRAY_SIZE], int *cursor, int row, int x, int y)
	** 옵션의 사운드 조절 메뉴를 구현하기 위해 제작된 특수 함수

	==

	13) int stop_BGM(void)
	14) int stop_SE(void)

	각 채널에 재생되는 사운드 중지

	==

	15) int fmod_close(void)

	FMOD 시스템 해제

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
		"  BGM: ◀ 0 ▶  ",
		"   SE: ◀ 0 ▶  ",
		"   -- Exit --"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기

	sprintf(str[0], "  BGM: ◀ %d ▶  ", bgm_volume);
	sprintf(str[1], "   SE: ◀ %d ▶  ", se_volume);
	
	for (i = y; i < y + row; i++) {
		for (j = x; j < x + 20; j++) {
			ScreenPrint(j, i, " ");
		}
	}

	//'z'키를 누르면 반복문 종료
	while (keyInput(&cursor, 0, row - 1)) {
		for (i = 0; i < row; i++) {
			//i = 0부터 row - 1까지 반복
			
			V1 = (cursor == i) ? 12 : 15;
			setColor(V1, V2);
			//LPRINT(x, y + i, str[i]); //배열 그리기
			ScreenPrint(x, y + i, str[i]);
		}
		//속도 제한
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
	sprintf(str[0], "  BGM: ◀ %d ▶  ", bgm_volume);
	sprintf(str[1], "   SE: ◀ %d ▶  ", se_volume);

	for (i = 0; i < row; i++) {
		//i = 0부터 row - 1까지 반복
		//커서 위치와 같다면 텍스트 색상을 붉은색으로 변경, 아니면 흰색으로
		V1 = (*cursor == i) ? 14 : 15;
		setColor(V1, V2);
		//LPRINT(x, y + i, str[i]); //배열 그리기
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