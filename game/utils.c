#include <stdio.h>
#include "utils.h"
/*
	** utils.c : ��ƿ �Լ� ����

	1) COORD SCR_LU (LEFT_UP)
	SetPos()�� ���Ǵ� X, Y ��ǥ ���۷��� (0, 0)

	2) COORD SCR_LB (LEFT_BOTTOM)
	SetPos()�� ���Ǵ� X, Y ��ǥ ���۷��� (0, HEIGHT_SIZE - 1)

	3) COORD GetPos(void)
	���� �ܼ� â�� Ŀ�� ��ġ�� COORD Ÿ��(short x, short y)���� ��ȯ�ϴ� ���۷���

	4) void SetPos(COORD pos)
	COORD Ÿ���� �����͸� �Ű������� �޾� �ܼ� â�� Ŀ�� ��ġ�� �����ϴ� ���۷���

	5) void gotoxy(int x, int y)
	SetPos()�� ����� �ܼ� â Ŀ���� x, y ��ǥ �� ���� (�Ű������� int�� ����)

	6) int isKeyDown(int key)
	Ű�� ���ȴ��� Ȯ���ϴ� �Լ�

	7) void CursorView(char show)
	�ܼ� â Ŀ���� �����/���̱� ó��
	0: �����, 1: ���̱�

	8) void InitConsole(void)
	���� ���� �� �ʿ��� �ܼ� â ������ ����
	+���� ���� ��� ��Ȱ��ȭ (���� ��� ��ȯ���� ���� ���� ����)
	+��ũ�ѹ� ��Ȱ��ȭ
	+�ִ�ȭ ��ư ��Ȱ��ȭ
	+ũ�� ���� ��Ȱ��ȭ

	9) void connect_notifier(int mode)
	���ͳ� ���� ��� �� ���� �ϴܿ� "[���� ��]" ���� ǥ��
	WIDTH_SIZE, HEIGHT_SIZE ���� ���� �������� ��ġ ����

	10) void xprint_init(int x, int y, int x_size, int y_size)
	xprint() �Լ��� ���Ǵ� �� ����

	-- xprint_init()
	x, y: x, y ��ǥ
	x_size, y_size: x, y ��ǥ�� �������� ũ�� ����


	11) void xprint(const char* string, int delay, int mode)
	��ȭ���� ��¿� �Լ�
	�� �Լ��� ����ϱ� ���� xprint_init()�� ���� ����ؾ� ��.

	-- xprint(const char* string, int delay, int mode)
	string: ���
	delay: ���� �ð�
	mode: 1 - ��� �� ����,
		  0 - ������ ����

		  
	12) void ScreenFlipping(void)
	13) void ScreenClear(void)
	14) void ScreenRelease(void)
	15) void ScreenPrint(int x, int y, char* string)
	���� ���۸� ���� �Լ�
*/

//�ܺο��� ����Ϸ��� xprint_init()���� ����
static int xprint_target_pos_X;
static int xprint_target_pos_Y;
static int xprint_X_size;
static int xprint_Y_size;

HANDLE hBuffer[2];  //���� �ڵ�
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
		printf("Error: GetPos() �Լ� ���� �� ������ �߻��߽��ϴ�.\nError Code: %u\n", (unsigned int)GetLastError());
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

	//���� ���� �ʱ�ȭ (0: �⺻, 1: ���� ����)
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

//�ؽ�Ʈ, ���� ��� ���� �Լ�
MYDLL void setColor(int color, int bgcolor) {
	SetConsoleTextAttribute(hBuffer[nScreenIndex], ((bgcolor & 0xf) << 4) | (color & 0xf));
}

//�ؽ�Ʈ ���� ���� �Լ�
MYDLL void setFontColor(int color) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hBuffer[nScreenIndex], &info);
	SetConsoleTextAttribute(hBuffer[nScreenIndex], (info.wAttributes & 0xf0) | (color & 0xf));
}

//��� ���� ���� �Լ�
MYDLL void setBgColor(int bgcolor) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hBuffer[nScreenIndex], &info);
	SetConsoleTextAttribute(hBuffer[nScreenIndex], ((bgcolor & 0xf) << 4) | (info.wAttributes & 0xf));
}

//�ؽ�Ʈ ���� ȹ�� �Լ�
MYDLL int getFontColor(void) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hBuffer[nScreenIndex], &info);
	return info.wAttributes & 0xf;
}

//��� ���� ȹ�� �Լ�
MYDLL int getBgColor(void) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hBuffer[nScreenIndex], &info);
	return (info.wAttributes & 0xf0) >> 4;
}

//�ؽ�Ʈ �÷� �⺻�� 
MYDLL void resetFontColor(void) {
	setFontColor(7);
}

//���� �⺻��
MYDLL void resetBgColor(void) {
	setBgColor(0);
}

//�ؽ�Ʈ, ���� �⺻��
MYDLL void resetColor(void) {
	setColor(7, 0);
}

MYDLL void connect_notifier(int mode) {
	char buf[32];
	char *message = "[���� ��]";
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

	//(x) ~ (x+x_size) X (y) ~ (y+y_size) ��ŭ�� ȭ�� ���� �����
	for (int i = 0; i < xprint_Y_size; i++) {
		gotoxy(xprint_target_pos_X, xprint_target_pos_Y + i);

		for (int j = 0; j < xprint_X_size; j++) {
			printf(" ");
		}
	}

	//x, y ��ǥ �ʱ�ȭ
	gotoxy(xprint_target_pos_X, xprint_target_pos_Y);
	
	for (int i = 0; i < str; i++) {
		//�ѱ� ���� ���� (�ѱ� ���� �� 2����Ʈ�� ���)
		//�ѱ� �ڵ�� 0x80 �̻��� ���� ������ �����Ƿ� ������ &�� ����� �ش� ���ڰ� �ѱ����� �ƴ����� Ȯ��.
		//0�̸�(0000 0000) �ѱ� X, 0�� �ƴϸ�(1000 0000�̸�) �ѱ�
		//!= 0 ��� == 0x80�� ����
		if ((string[i] & 0x80) != 0) {
			printf("%c%c", string[i], string[i + 1]);
			i++;
		}
		//�ٹٲ� ���ڸ� ���� ���
		else if (string[i] == '\n') {
			y++;
			gotoxy(x, y);
		}
		//�ѱ�, �ٹٲ� ���ڰ� �ƴϸ� 1����Ʈ ���
		else {
			printf("%c", string[i]);
		}

		//Ű���� �Է� ���� �� ���� ������ �ѱ��
		if (_kbhit()) {
			delay = 0; _getch();
		}
		
		//������ ȿ��
		Sleep(delay);
	}

	//mode�� ��� �� �������� �����Ǿ� ������ ��� ��� �� Ŀ�� ������ + ��ٸ���.
	if (mode == 1) {
		CursorView(1);
		_getch();
		CursorView(0);
	}

	//�ڵ� �ٹٲ�
	printf("\n");
}