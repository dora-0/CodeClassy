#include <stdio.h>
#include "utils.h"
/*
	** utils.c : 유틸 함수 모음

	1) COORD SCR_LU (LEFT_UP)
	SetPos()에 사용되는 X, Y 좌표 레퍼런스 (0, 0)

	2) COORD SCR_LB (LEFT_BOTTOM)
	SetPos()에 사용되는 X, Y 좌표 레퍼런스 (0, HEIGHT_SIZE - 1)

	3) COORD GetPos(void)
	현재 콘솔 창의 커서 위치를 COORD 타입(short x, short y)으로 반환하는 레퍼런스

	4) void SetPos(COORD pos)
	COORD 타입의 데이터를 매개변수로 받아 콘솔 창의 커서 위치를 지정하는 레퍼런스

	5) void gotoxy(int x, int y)
	SetPos()를 사용한 콘솔 창 커서의 x, y 좌표 값 지정 (매개변수를 int로 받음)

	6) int isKeyDown(int key)
	키가 눌렸는지 확인하는 함수

	7) void CursorView(char show)
	콘솔 창 커서의 숨기기/보이기 처리
	0: 숨기기, 1: 보이기

	8) void InitConsole(void)
	게임 실행 시 필요한 콘솔 창 프리셋 지정
	+빠른 편집 모드 비활성화 (선택 모드 전환으로 인한 멈춤 방지)
	+스크롤바 비활성화
	+최대화 버튼 비활성화
	+크기 조정 비활성화

	9) void connect_notifier(int mode)
	인터넷 연결 사용 시 우측 하단에 "[연결 중]" 문구 표시
	WIDTH_SIZE, HEIGHT_SIZE 값에 따라 동적으로 위치 지정

	10) void xprint_init(int x, int y, int x_size, int y_size)
	xprint() 함수에 사용되는 값 지정

	-- xprint_init()
	x, y: x, y 좌표
	x_size, y_size: x, y 좌표를 기준으로 크기 지정


	11) void xprint(const char* string, int delay, int mode)
	대화상자 출력용 함수
	이 함수를 사용하기 전에 xprint_init()을 먼저 사용해야 함.

	-- xprint(const char* string, int delay, int mode)
	string: 출력
	delay: 지연 시간
	mode: 1 - 출력 후 멈춤,
		  0 - 멈추지 않음

		  
	12) void ScreenFlipping(void)
	13) void ScreenClear(void)
	14) void ScreenRelease(void)
	15) void ScreenPrint(int x, int y, char* string)
	다중 버퍼링 관련 함수
*/

//외부에서 사용하려면 xprint_init()에서 설정
static int xprint_target_pos_X;
static int xprint_target_pos_Y;
static int xprint_X_size;
static int xprint_Y_size;

HANDLE hBuffer[2];  //버퍼 핸들
int nScreenIndex = 0;

COORD SCR_LU = {
		0,
		0
};

COORD SCR_LB = {
		0,
		HEIGHT_SIZE - 1
};

MYDLL COORD GetPos(void) {
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (GetConsoleScreenBufferInfo(hBuffer[nScreenIndex], &cbsi))
		return cbsi.dwCursorPosition;

	else {
		// The function failed. Call GetLastError() for details.
		printf("Error: GetPos() 함수 실행 중 오류가 발생했습니다.\nError Code: %u\n", (unsigned int)GetLastError());
		COORD invalid = { 0, 0 };
		return invalid;
	}
}

MYDLL void SetPos(COORD pos) {
	SetConsoleCursorPosition(hBuffer[nScreenIndex], pos);
}

MYDLL void gotoxy(int x, int y) {
	COORD pos = { x, y };
	SetPos(pos);
}

MYDLL int isKeyDown(int key) {
	return ((GetAsyncKeyState(key) & 0x8000) != 0);
}

MYDLL void CursorView(char show) {
	CONSOLE_CURSOR_INFO ConsoleCursor;
	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;

	SetConsoleCursorInfo(hBuffer[0], &ConsoleCursor);
	SetConsoleCursorInfo(hBuffer[1], &ConsoleCursor);
}

MYDLL void InitConsole(void) {
	HWND hwnd = GetConsoleWindow();
	DWORD style = GetWindowLong(hwnd, GWL_STYLE);
	DWORD dwMode = 0;

	//다중 버퍼 초기화 (0: 기본, 1: 새로 생성)
	hBuffer[0] = GetStdHandle(STD_OUTPUT_HANDLE);
	hBuffer[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	GetConsoleMode(hBuffer[0], &dwMode);

	dwMode &= ~ENABLE_QUICK_EDIT_MODE;

	SetConsoleMode(hBuffer[0], dwMode | ENABLE_EXTENDED_FLAGS);
	SetConsoleMode(hBuffer[1], dwMode | ENABLE_EXTENDED_FLAGS);

	style &= ~WS_MAXIMIZEBOX &~WS_SIZEBOX &~WS_HSCROLL &~WS_VSCROLL;
	SetWindowLong(hwnd, GWL_STYLE, style);
	SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
}

MYDLL void ScreenFlipping(void) {
	Sleep(10);
	nScreenIndex = !nScreenIndex;
	SetConsoleActiveScreenBuffer(hBuffer[nScreenIndex]);
}


MYDLL void ScreenClear(void) {
	COORD Coor = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD cCharsWritten;
	DWORD dwConSize;
	GetConsoleScreenBufferInfo(hBuffer[nScreenIndex], &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hBuffer[nScreenIndex], ' ', dwConSize, Coor, &cCharsWritten);
}


MYDLL void ScreenRelease(void) {
	CloseHandle(hBuffer[1]);
}


MYDLL void ScreenPrint(int x, int y, char* string) {
	DWORD dw;
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition(hBuffer[nScreenIndex], CursorPosition);
	WriteFile(hBuffer[nScreenIndex], string, strlen(string), &dw, NULL);

}

//텍스트, 배경색 모두 변경 함수
MYDLL void setColor(int color, int bgcolor) {
	SetConsoleTextAttribute(hBuffer[nScreenIndex], ((bgcolor & 0xf) << 4) | (color & 0xf));
}

//텍스트 색상 변경 함수
MYDLL void setFontColor(int color) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hBuffer[nScreenIndex], &info);
	SetConsoleTextAttribute(hBuffer[nScreenIndex], (info.wAttributes & 0xf0) | (color & 0xf));
}

//배경 색상 변경 함수
MYDLL void setBgColor(int bgcolor) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hBuffer[nScreenIndex], &info);
	SetConsoleTextAttribute(hBuffer[nScreenIndex], ((bgcolor & 0xf) << 4) | (info.wAttributes & 0xf));
}

//텍스트 색상 획득 함수
MYDLL int getFontColor(void) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hBuffer[nScreenIndex], &info);
	return info.wAttributes & 0xf;
}

//배경 색상 획득 함수
MYDLL int getBgColor(void) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hBuffer[nScreenIndex], &info);
	return (info.wAttributes & 0xf0) >> 4;
}

//텍스트 컬러 기본값 
MYDLL void resetFontColor(void) {
	setFontColor(7);
}

//배경색 기본값
MYDLL void resetBgColor(void) {
	setBgColor(0);
}

//텍스트, 배경색 기본값
MYDLL void resetColor(void) {
	setColor(7, 0);
}

MYDLL void connect_notifier(int mode) {
	char buf[32];
	char *message = "[연결 중]";
	sprintf(buf, "%%%ds", WIDTH_SIZE);

	COORD currentPos = GetPos();

	SetPos(SCR_LB);
	printf("\33[2K\r"); //delete current line

	if (mode == 1) printf(buf, message); //%[WIDTH_SIZE]s
	SetPos(currentPos);
	//Sleep(500); //function test
	return;
}

MYDLL void xprint_init(int x, int y, int x_size, int y_size) {
	xprint_target_pos_X = x;
	xprint_target_pos_Y = y;
	xprint_X_size = x_size;
	xprint_Y_size = y_size;
}

MYDLL void xprint(const char *string, int delay, int mode) {
	int str = strlen(string); //strSize Fix
	int x = xprint_target_pos_X;
	int y = xprint_target_pos_Y;

	//(x) ~ (x+x_size) X (y) ~ (y+y_size) 만큼의 화면 영역 지우기
	for (int i = 0; i < xprint_Y_size; i++) {
		gotoxy(xprint_target_pos_X, xprint_target_pos_Y + i);

		for (int j = 0; j < xprint_X_size; j++) {
			printf(" ");
		}
	}

	//x, y 좌표 초기화
	gotoxy(xprint_target_pos_X, xprint_target_pos_Y);
	
	for (int i = 0; i < str; i++) {
		//한글 깨짐 방지 (한글 감지 시 2바이트씩 출력)
		//한글 코드는 0x80 이상의 값을 가지고 있으므로 연산자 &를 사용해 해당 문자가 한글인지 아닌지를 확인.
		//0이면(0000 0000) 한글 X, 0이 아니면(1000 0000이면) 한글
		//!= 0 대신 == 0x80도 성립
		if ((string[i] & 0x80) != 0) {
			printf("%c%c", string[i], string[i + 1]);
			i++;
		}
		//줄바꿈 문자를 만날 경우
		else if (string[i] == '\n') {
			y++;
			gotoxy(x, y);
		}
		//한글, 줄바꿈 문자가 아니면 1바이트 출력
		else {
			printf("%c", string[i]);
		}

		//키보드 입력 감지 시 문자 끝까지 넘기기
		if (_kbhit()) {
			delay = 0; _getch();
		}
		
		//딜레이 효과
		Sleep(delay);
	}

	//mode가 출력 후 멈춤으로 설정되어 있으면 모두 출력 후 커서 깜빡임 + 기다리기.
	if (mode == 1) {
		CursorView(1);
		_getch();
		CursorView(0);
	}

	//자동 줄바꿈
	printf("\n");
}