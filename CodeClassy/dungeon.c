#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
/*
	** dungeon.c : 던전 이벤트 처리

	1) static int (*d_event[6])()
	** 함수 포인터 형태의 던전 이벤트 호출 함수.
	사용: d_event[enum EventFuncList];

	===
	
	2) int d_EventInit(void)
	** 던전 이벤트 초기화 함수. (제일 먼저 호출됨)
	현재 플레이어 위치의 mask 값에 따라 다양한 이벤트를 실행.

	===

	3) int d_randomEvent(void) (미사용)
	배틀 이벤트 발동 시 일정 확률로 아이템 획득 이벤트를 발생시키기 위한 함수.
	시간 부족으로 구현 X

	===

	4) void d_Message_Blocked(void)
	막힌 방향으로 이동 시도 시 출력 메시지

	===

	5) void d_movePos(int mode)
	** 기존의 map.c - move_position()이 d_movePos()로 변경
	** 사용자의 좌표를 이동시키는 함수
	인자값 map은 enum map_position_move_mode의 데이터를 사용한다.

	사용자가 위치한 좌표의 mask 값이 계단 (Up or Down)이라면 좌표 이동 시도 시 해당 mask 값을 유지. (계속 통행할 수 있도록)
	아니라면 이동 시도 시 mask 값을 -1로 변경. (이벤트 없음)

	사용자가 가고자 하는 방향의 방 정보 중 is_blocked 값이 1(통행 불가)이라면,
	해당 위치로 이동할 수 없게 하며 아니라면 사용자의 좌표 값을 변경하고
	해당 방의 is_first 값을 0으로 만든다.

	이동 후, move_anim()을 호출한다.

	+ MOVE_AUTO 옵션 추가
	층을 바꿀 때 계단 위치로 바로 이동하게 되면 버그가 생기므로 MOVE_AUTO 옵션으로 이동한다.
	MOVE_AUTO 옵션은 사용자가 현재 위치한 좌표의 상하좌우를 체크해 열린 방향의 쪽으로 자동으로 이동한다.

	모든 방은 두 갈래 이상의 길이 없으므로 계단 진입 시 MOVE_AUTO 옵션으로 계단으로부터 한칸 떨어져 나오게 함.

	===

	6) int _default(void)
	** 던전 이벤트 : 기본 (d_event() 함수 포인터로 호출)

	이동 위치 지정

	===

	7) int _battle(void)
	** 던전 이벤트 : 전투 (d_event() 함수 포인터로 호출)

	전투 발생 이벤트

	===

	8) int _item(void)
	9) int _special(void)

	아이템 획득 이벤트 (미구현)
	특수 이벤트 (미구현)

	===

	10) int _up(void)
	11) int _down(void)

	계단 올라가기/내려가기 이벤트

	-> 층 이동

*/

#define _NULL -1
extern struct MAP *map[6];
static int (*d_event[6])() = { _default, _battle, _item, _up, _down, _special }; //함수 포인터

const enum EventFuncList {
	DEFAULT = 0,
	BATTLE = 1,
	ITEM = 2,
	S_UP = 3,
	S_DOWN = 4,
	SPECIAL = 5
};

const enum EventMaskList {
	NONE = -1,
	BATTLE_ZONE = 0,
	BLOCKED = 1,
	SP_EVENT = 7,
	STAIR_UP = 8,
	STAIR_DOWN = 9
};

//Event List:
//전투 이벤트
//특수 이벤트(특정 방 전용)
//이동 대기 이벤트

int d_EventInit(void) {
	switch (map[MAPID]->level[USERY][USERX]->mask) {
	case STAIR_UP:
		if (d_event[S_UP]()) //반환값이 1이면 (올라간다 선택)
			(MAPID)++;
		d_movePos(MOVE_AUTO); //자동으로 열린 방향으로 1칸 이동
		return 1;
		break;
	case STAIR_DOWN:
		if (d_event[S_DOWN]()) //반환값이 1이면 (내려간다 선택)
			(MAPID)--;
		d_movePos(MOVE_AUTO); //자동으로 열린 방향으로 1칸 이동
		return 1;
		break;
	case SP_EVENT:
		return d_event[SPECIAL]();
		break;
	case BATTLE_ZONE:
		//d_event[d_randomEvent()]();
		if (!d_event[BATTLE]()) {
			return 0;
		}
	case NONE:
		return d_event[DEFAULT]();
		break;
	}

	return 1;
}

int d_randomEvent(void) { //미사용
	int EventFunc;

	srand((unsigned)time(NULL));
	EventFunc = rand() % 2 + 1;

	return EventFunc;
}

void d_Message_Blocked(void) { //막힌 방향으로 이동 시도 시 출력
	drawMenuBorder(24, 8);
	xprint_init(27, 10, 10, 5);
	xprint("이 방향으로는\n이동할 수 없는 것 같다.", 30, 1);
}

void d_movePos(int mode) {
	switch (map[MAPID]->level[USERY][USERX]->mask) {
	case STAIR_UP:
	case STAIR_DOWN:
		break;
	default:
		map[MAPID]->level[USERY][USERX]->mask = NONE;
	}

	switch (mode) {
	case MOVE_UP:
		if (map[MAPID]->level[(USERY)-1][USERX]->mask == BLOCKED) {
			d_Message_Blocked();
		}			
		else {
			map[MAPID]->level[(USERY)-1][USERX]->is_first = 0;
			(USERY)--;
		}
		break;
	case MOVE_DOWN:
		if (map[MAPID]->level[(USERY)+1][USERX]->mask == BLOCKED) {
			d_Message_Blocked();
		}
		else {
			map[MAPID]->level[(USERY)+1][USERX]->is_first = 0;
			(USERY)++;
		}
		break;
	case MOVE_LEFT:
		if (map[MAPID]->level[USERY][(USERX)-1]->mask == BLOCKED) {
			d_Message_Blocked();
		}
		else {
			map[MAPID]->level[USERY][(USERX)-1]->is_first = 0;
			(USERX)--;
		}
		break;
	case MOVE_RIGHT:
		if (map[MAPID]->level[USERY][(USERX)+1]->mask == BLOCKED) {
			d_Message_Blocked();
		}
		else {
			map[MAPID]->level[USERY][(USERX)+1]->is_first = 0;
			(USERX)++;
		}
		break;
	case MOVE_AUTO:
		if (map[MAPID]->level[USERY][(USERX)+1]->mask != BLOCKED) {
			map[MAPID]->level[USERY][(USERX)+1]->is_first = 0;
			(USERX)++;
		}
			
		else if (map[MAPID]->level[USERY][(USERX)-1]->mask != BLOCKED) {
			map[MAPID]->level[USERY][(USERX)-1]->is_first = 0;
			(USERX)--;
		}
			
		else if (map[MAPID]->level[(USERY)+1][USERX]->mask != BLOCKED) {
			map[MAPID]->level[(USERY)+1][USERX]->is_first = 0;
			(USERY)++;
		}
			
		else if (map[MAPID]->level[(USERY)-1][USERX]->mask != BLOCKED) {
			map[MAPID]->level[(USERY)-1][USERX]->is_first = 0;
			(USERY)--;
		}
			
		break;
	default:
		break;
	}

	//페이드 아웃-페이드 인 애니메이션
	move_anim();
}


int _default(void) {
	static int cursor = STR1;
	//default x, y position
	int x = 2, y = 8;
	char str[][MAX_ARRAY_SIZE] = {
		"> Up",
		"  Down",
		"  Left",
		"  Right",
		"  ",
		"  Exit"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기

	system("cls");

	show_status_init(1, HEIGHT_SIZE - 2, 26, HEIGHT_SIZE - 2);
	show_status(1, 0);

	show_current_map();
	LPRINT(x, 1, "어디로 갈까?");
	show_select_menu(x, y, &cursor, str, 10, row);

	switch (cursor) {
	case STR1:
		d_movePos(MOVE_UP);
		break;
	case STR2:
		d_movePos(MOVE_DOWN);
		break;
	case STR3:
		d_movePos(MOVE_LEFT);
		break;
	case STR4:
		d_movePos(MOVE_RIGHT);
		break;
	case STR6:
		return 0;
		break;
	default:
		return d_event[DEFAULT](); //빈칸이 눌렸을 경우 Default 이벤트 재호출 (이중 전투 방지)
		break;
	}

	return 1;
}

int _battle(void) {
	int enemyCount = 0;
	int enemyLevel = 0;

	srand((unsigned)time(NULL));
	enemyCount = rand() % 3 + 1;

	switch (enemyCount) { //적의 수 1 ~ 3 랜덤 지정
	case 1:
		setCurrentEnemies(1, _NULL, _NULL); //_NULL == -1, -1로 입력 시 비활성화 된 것으로 판단
		break;
	case 2:
		setCurrentEnemies(1, 1, _NULL);
		break;
	case 3:
		setCurrentEnemies(1, 1, 1);
		break;
	}
	
	ResetEscapeClickFlag();
	while (battle_screen(enemyCount)) {
		if (!AttackProcess(AP_EtP)) { //전투 프로세스 (ETP 모드)에서 반환값이 0이라면 (=게임 오버)
			return 0; //_battle() -> d_EventInit()에서 0 반환 (던전 나가기)
		}
	}
	freeCurrentEnemies();
	return 1;
}

int _item(void) {
	printf("Item Event!!\n"); //아이템 획득 이벤트 (미구현)
	Sleep(500);
	return 1;
}

int _up(void) { //계단 올라가는 이벤트
	static int cursor = STR2;
	//default x, y position
	int x = 2, y = 8;
	char str[][MAX_ARRAY_SIZE] = {
		"  올라간다",
		"> 돌아간다"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기

	system("cls");

	show_status_init(1, HEIGHT_SIZE - 2, 26, HEIGHT_SIZE - 2);
	show_status(1, 0);

	show_current_map();
	LPRINT(x, 1, "올라가는 계단이 있다.");
	show_select_menu(x, y, &cursor, str, 10, row);

	switch (cursor) {
	case STR1:
		return 1;
		break;
	case STR2:
		break;
	}

	return 0;
}

int _down(void) { //계단 내려가는 이벤트
	static int cursor = STR2;
	//default x, y position
	int x = 2, y = 8;
	char str[][MAX_ARRAY_SIZE] = {
		"  내려간다",
		"> 돌아간다"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기

	system("cls");

	show_status_init(1, HEIGHT_SIZE - 2, 26, HEIGHT_SIZE - 2);
	show_status(1, 0);

	show_current_map();
	LPRINT(x, 1, "내려가는 계단이 있다.");
	show_select_menu(x, y, &cursor, str, 10, row);

	switch (cursor) {
	case STR1:
		return 1;
		break;
	case STR2:
		break;
	}

	return 0;
}

int _special(void) { //특수 이벤트 (미구현)
	printf("SP Event!!\n");
	Sleep(500);
	return d_event[DEFAULT]();
}