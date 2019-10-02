#include <stdio.h>
#include "main.h"
#include "sound.h"
/*
	** ui.c : 유저 인터페이스

	1) static int ref_speed
	** 리프레시 속도 (높을수록 느려짐)

	show_select_menu() 함수의 구현은 일정 시간마다 계속 화면을 새로고침하면서
	특정 문자를 그리는 방식으로 구성되어 있으므로,
	중간에 Sleep을 주지 않으면 매우 빠른 속도로 새로고침되어 PC의 자원 낭비가 심해진다.

	따라서 속도 제한을 목적으로 ref_speed를 설정할 수 있다.
	정적 전역 변수 ref_speed는 이 파일에서만 접근이 가능하며, 외부에서 접근 시
	get_ref_speed(), set_ref_speed() 함수 사용 필요

	* set_ref_speed()의 인자값으로 change_value() 함수 값을 넣어 사용자에게 값을 입력하도록 할 수 있다.
	change_value()는 디버그용 함수로 입력값 검사가 이루어지지 않기 때문에 실제 릴리즈 시 사용 X
	(해당 부분은 추후 사용자 입력 없이 방향키로 조절하도록 구현)

	===

	2) void show_select_menu(int x, int y, int *cursor, char str[][MAX_ARRAY_SIZE], int col, int row)
	** 메뉴 선택을 구현하는 함수

	gotoxy와 미리 정의된 매크로 LPRINT를 사용해
	사용자의 방향키 입력을 받을 수 있는 메뉴를 구성한다.

	x, y: 기준점 좌표
	cursor: cursor로 사용할 int 변수의 주소
	str: 메뉴로 사용할 배열
	col: 메뉴 영역의 가로 길이 (화면 지우기에 사용됨)
	row: sizeof로 계산한 배열의 세로 길이 (화면 지우기에 사용됨)

	키 입력 감지로 keyInput() 함수를 사용한다.

	사용자가 선택한 항목은 붉은 색으로 강조 처리한다.

	===

	3) void menu(void)
	4) void menu_options(void)
	** 메뉴를 출력하는 함수

	show_select_menu() 함수를 사용해 메뉴를 출력한다.
	cursor 값으로 enum menu_cursor의 값을 사용할 수 있다. (STR1~STR10)
	(STR1: 0, STR2: 1, ..., STR10: 9)

	===

	5) void drawMenuBorder(int x, int y)
	6) void drawBigMenuBorder(int x, int y)
	** 테두리를 그려주는 함수
	입력받은 x, y의 값을 기준으로 테두리를 그려준다.

	==

	7) int menu_init(void)
	** 초기화 화면을 출력하는 함수

	새 게임을 시작하거나 기존 데이터를 서버에서 불러오는 메뉴를 출력한다.
	다중 버퍼를 사용해 자연스러운 메뉴 전환이 가능하도록 구현되었다. (ScreenPrint(), ScreenFlipping(), ScreenClear())

	==

	8) void menu_pause(void)
	** 일시정지 메뉴를 출력하는 함수

	ESC 키를 누르면 일시정지 메뉴를 출력한다. (xprint() 함수 작동 중엔 제외)
	다중 버퍼를 사용해 자연스러운 메뉴 전환이 가능하도록 구현되었다. (ScreenPrint(), ScreenFlipping(), ScreenClear())

	==

	9) int keyInput(int *cursor, int min_pos, int max_pos)
	** 키보드로부터 특정 키 입력을 감지하는 함수

	사용자가 방향키를 누를 때, 포인터 cursor가 가리키는 값을 조정한다.
	min_pos: *cursor의 최소 값 (이 값보다 낮으면 더 감소하지 않게 함)
	max_pos: *cursor의 최대 값 (이 값보다 높으면 더 증가하지 않게 함)

	===

	10) int get_ref_speed(void)
	11) void set_ref_speed(int speed)
	** 외부에서 ref_speed 값 확인 및 수정 시 사용하는 함수

	정적 전역 변수인 ref_speed를 외부에서 확인 또는 수정 시 사용할 수 있다.
*/

static int ref_speed = 60;

void show_select_menu(int x, int y, int *cursor, char str[][MAX_ARRAY_SIZE], int col, int row) {
	int i = 0, j = 0;
	//setColor의 인자로 사용
	int V1 = 15, V2 = 0;

	//지정된 화면 영역 지우기 (y 부터 y + row까지, x부터 x + col까지)
	for (i = y; i < y + row; i++) {
		for (j = x; j < x + col; j++) {
			ScreenPrint(j, i, " ");
		}
	}

	//'z'키를 누르면 반복문 종료
	while (keyInput(cursor, 0, row - 1)) {
		//배열 str의 첫번째 값 지우기 ('>'가 위치할 곳)
		for (i = 0; i < row; i++) {
			str[i][0] = ' ';
		}

		//선택 중인 커서 위치에 '>' 삽입
		str[*cursor][0] = '>';

		for (i = 0; i < row; i++) {
			//i = 0부터 row - 1까지 반복
			//커서 위치와 같다면 텍스트 색상을 붉은색으로 변경, 아니면 흰색으로
			V1 = (*cursor == i) ? 12 : 15;
			setColor(V1, V2);
			//LPRINT(x, y + i, str[i]); //배열 그리기
			ScreenPrint(x, y + i, str[i]);
		}
		//속도 제한
		Sleep(ref_speed);
	}
}

int menu(void) {
	static int cursor = STR1;
	//default x, y position
	int x = 33, y = 15;
	char str[][MAX_ARRAY_SIZE] = {
		"> 게임 시작",
		"  옵션",
		"  나가기"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기
	
	system("cls");
	gotoxy(0, HEIGHT_SIZE - 1);
	printf("Version %d.%d.%d (Build Type: %s)", VERSION, MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE);

	LPRINT(x-8, y-6,	"=============================");
	LPRINT(x-8, y-5,	" P R O J E C T   C L A S S Y");
	LPRINT(x-8, y-4,	"=============================");

	//x = 33, y = 15, int cursor, 배열 str, x 크기 20, y 크기 3
	show_select_menu(x, y, &cursor, str, 20, row);

	switch (cursor) {
	case STR1:
		gamectl(GAME_START);
		break;
	case STR2:
		menu_options();
		break;
	case STR3:
		return 0;
		break;
	default:
		break;
	}

	return 1;
}

void drawMenuBorder(int x, int y) {
	ScreenPrint(x, y + 0, "┌─────────────────────────────┐");
	ScreenPrint(x, y + 1, "│                             │");
	ScreenPrint(x, y + 2, "│                             │");
	ScreenPrint(x, y + 3, "│                             │");
	ScreenPrint(x, y + 4, "│                             │");
	ScreenPrint(x, y + 5, "│                             │");
	ScreenPrint(x, y + 6, "│                             │");
	ScreenPrint(x, y + 7, "└─────────────────────────────┘");

}

void drawBigMenuBorder(int x, int y) {
	ScreenPrint(x, y + 0, "┌──────────────────────────────────────┐");
	ScreenPrint(x, y + 1, "│                                      │");
	ScreenPrint(x, y + 2, "│                                      │");
	ScreenPrint(x, y + 3, "│                                      │");
	ScreenPrint(x, y + 4, "│                                      │");
	ScreenPrint(x, y + 5, "│                                      │");
	ScreenPrint(x, y + 6, "│                                      │");
	ScreenPrint(x, y + 7, "│                                      │");
	ScreenPrint(x, y + 8, "│                                      │");
	ScreenPrint(x, y + 9, "│                                      │");
	ScreenPrint(x, y + 10, "└──────────────────────────────────────┘");

}

int menu_init(void) {
	static int cursor = STR1;
	static int cursor2 = STR2;
	int code = -1;
	//default x, y position
	int x = 30, y = 12;
	char str[][MAX_ARRAY_SIZE] = {
		"> 새 게임 시작",
		"  기존 데이터 불러오기",
		"  ",
		"  게임 종료"
	};

	char str2[][MAX_ARRAY_SIZE] = {
		"  예",
		"> 아니오"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기
	int row2 = sizeof(str2) / sizeof(str2[0]);

	ScreenFlipping();
	drawMenuBorder(24, 10);
	show_select_menu(x - 2, y, &cursor, str, 20, row);

	switch (cursor) {
	case STR1:
		ScreenClear();
		drawMenuBorder(24, 10);
		ScreenPrint(x, y, "새 게임을 시작합니다.\n");
		show_select_menu(x + 2, y + 2, &cursor2, str2, 20, row2);

		switch (cursor2) {
		case STR1:
			code = 0;
			ScreenClear();
			ScreenFlipping();
			joinUser();
			return 0;
			break;
		case STR2:
			code = 1;
			break;
		}
		break;
	case STR2:
		ScreenClear();
		ScreenFlipping();
		Login();
		return 0;
		break;
	case STR3:
		break;
	case STR4:
		break;
	case _ESC:
		break;
	default:
		break;
	}
	ScreenClear();
	ScreenFlipping();
	return code;
}

void menu_options(void) {
	int cursor = STR1;
	//default x, y position
	int x = 33, y = 15;
	char str[][MAX_ARRAY_SIZE] = {
		"> 사운드 설정",
		"  돌아가기"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기

	system("cls");

	LPRINT(x-8, y-6,	"=============================");
	LPRINT(x-8, y-5,	"           OPTIONS");
	LPRINT(x-8, y-4,	"=============================");

	show_select_menu(x, y, &cursor, str, 20, row);

	switch (cursor) {
	case STR1:
		while(toggleSound(31, 16));
		break;
	case STR2:
		break;
	default:
		break;
	}
}

void menu_pause(void) {
	int cursor = STR1;
	//default x, y position
	int x = 30, y = 12;
	char str[][MAX_ARRAY_SIZE] = {
		"> 사운드 설정",
		"  ",
		"  닫기"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기

	ScreenFlipping();
	drawMenuBorder(24, 10);
	show_select_menu(x, y, &cursor, str, 20, row);

	switch (cursor) {
	case STR1:
		while (toggleSound(31, 13));
		break;
	case STR3:
		break;
	case _ESC:
		break;
	default:
		break;
	}
	ScreenClear();
	ScreenFlipping();
}


int keyInput(int *cursor, int min_pos, int max_pos) {
	int key = 0;
	setColor(15, 0);

	if (_kbhit()) {
		key = _getch();

		//UP DOWN LEFT RIGHT
		if (key == EXTEND_KEY) {
			key = _getch();

			switch (key) {
			case KEY_LEFT:
			case KEY_UP:
				if (*cursor > min_pos) (*cursor)--; //cursor 값이 min_pos보다 크면 cursor 값 감소
				else (*cursor) = max_pos; //아니면 cursor 값을 max_pos로 변경
				break;
			case KEY_RIGHT:
			case KEY_DOWN:
				if (*cursor < max_pos) (*cursor)++; //cursor 값이 max_pos보다 작으면 cursor 값 증가
				else (*cursor) = min_pos; //아니면 cursor 값을 min_pos로 변경
				break;
			}
		}

		else if (key == KEY_Z) { //'z' 키를 누르면 0 반환 (show_select_menu()의 반복문 종료)
			return 0;
		}
		else if (key == KEY_ESC) {
			if (nScreenIndex != 1)
				menu_pause();
			else {
				(*cursor) = _ESC;
				return 0;
			}
		}
	}

	return 1;
}

int get_ref_speed(void) {
	return ref_speed;
}

void set_ref_speed(int speed) {
	ref_speed = speed;
}