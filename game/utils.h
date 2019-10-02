#pragma once
#pragma warning (disable:4996)
#include <conio.h>
#include <Windows.h>
#define LPRINT(x, y, str) gotoxy(x, y); printf("%s", str)
#define CHOP(x) x[strlen(x) - 1] = '\0'
#ifdef GAME_EXPORTS
#define MYDLL __declspec(dllexport)
#else
#define MYDLL __declspec(dllimport)
#endif

//버전 설정
#define VERSION 0
#define MAJOR_VERSION 2
#define MINOR_VERSION 0

#ifdef NDEBUG
#define BUILD_TYPE "Release"
#else
#define BUILD_TYPE "Debug"
#endif

//창 크기 설정
#define WIDTH_SIZE 80
#define HEIGHT_SIZE 30

enum ConsoleColor {
	CC_BLACK,
	CC_DARKBLUE,
	CC_DARKGREEN,
	CC_DARKCYAN,
	CC_DARKRED,
	CC_DARKMAGENTA,
	CC_DARKYELLOW,
	CC_GRAY,
	CC_DARKGRAY,
	CC_BLUE,
	CC_GREEN,
	CC_CYAN,
	CC_RED,
	CC_MAGENTA,
	CC_YELLOW,
	CC_WHITE,
	CC_LIGHTGRAY = 7,
	CC_ORIGINAL = 7,
	CC_ORIGINALFONT = 7,
	CC_ORIGINALBG = 0
};
MYDLL COORD SCR_LU;
MYDLL COORD SCR_LB;
MYDLL HANDLE hBuffer[2];
MYDLL int nScreenIndex;
MYDLL COORD GetPos(void);
MYDLL void SetPos(COORD pos);
MYDLL void gotoxy(int x, int y);
MYDLL int isKeyDown(int key);
MYDLL void CursorView(char show);
MYDLL void InitConsole(void);
MYDLL void ScreenFlipping(void);
MYDLL void ScreenClear(void);
MYDLL void ScreenRelease(void);
MYDLL void ScreenPrint(int x, int y, char* string);
MYDLL void setColor(int color, int bgcolor);
MYDLL void setFontColor(int color);
MYDLL void setBgColor(int bgcolor);
MYDLL int getFontColor(void);
MYDLL int getBgColor(void);
MYDLL void resetFontColor(void);
MYDLL void resetBgColor(void);
MYDLL void resetColor(void);
MYDLL void connect_notifier(int mode);

/*
	-- xprint_init(int x, int y, int x_size, int y_size)
	x, y: x, y 좌표
	x_size, y_size: x, y 좌표를 기준으로 크기 지정
*/
MYDLL void xprint_init(int x, int y, int x_size, int y_size);
/*
	-- xprint(const char* string, int delay, int mode)
	string: 출력
	delay: 지연 시간
	mode: 1 - 출력 후 멈춤,
		  0 - 멈추지 않음
*/
MYDLL void xprint(const char *string, int delay, int mode);