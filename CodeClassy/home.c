#include <stdio.h>
#include "main.h"
#include "sound.h"

void show_gold(void) { //보유 골드 표시
	gotoxy(2, 4);
	setFontColor(CC_YELLOW);
	printf("GOLD %dG", user.gold);
	setFontColor(CC_WHITE);
}

int EnterDungeonPrompt(void) { //던전 입장 확인
	int cursor = STR2;
	char str[][MAX_ARRAY_SIZE] = {
		"  예",
		"> 아니오"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기

	drawBigMenuBorder(18, 6);
	xprint_init(21, 7, 0, 0);
	xprint("던전에 입장한 후, 중도포기하거나\n전투불능이 된 경우\n처음부터 다시 시작해야 합니다.\n\n현재 진행 상황이\n자동으로 저장됩니다.\n던전에 입장하시겠습니까?", 30, 0);

	show_select_menu(47, 13, &cursor, str, 8, row);

	switch (cursor) {
	case STR1:
		gamectl(GAME_SAVE);
		return 1;
		break;
	case STR2:
		return 0;
		break;
	default:
		break;
	}

	return 0;
}

int home(void) {
	int cursor = STR1;
	//default x, y position
	int x = 2, y = 6;
	char str[][MAX_ARRAY_SIZE] = {
		"> 던전",
		"  상점",
		"  ",
		"  닉네임 변경",
		"  종료"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기

	system("cls");

	show_status_init(2, 2, 2, 3);
	show_status(1, 0);
	show_character_info();
	show_gold();

	show_select_menu(x, y, &cursor, str, 20, row);

	switch (cursor) {
	case STR1:
		if (EnterDungeonPrompt()) {
			play_BGM("BGM\\dungeon.ogg", FMOD_LOOP_NORMAL);
			gamectl(GAME_INIT);
			map_init();
			getEnemyInfo();
			move_anim();
			while (d_EventInit());
			freeEnemyInfo();
			map_clean();
			play_BGM("BGM\\home.ogg", FMOD_LOOP_NORMAL);
		}
		break;
	case STR2:
		break;
	case STR4:
		system("cls");
		CursorView(1);
		gotoxy(0, 0);
		printf("Set to: ");
		scanf("%20[^\n]", user.username);
		while (getchar() != '\n');
		CursorView(0);
		break;
	case STR5:
		return 0;
		break;
	default:
		break;
	}

	gamectl(GAME_SAVE); //서버에 게임 데이터 저장
	return 1;
}