#include <stdio.h>
#include <conio.h>
#include "main.h"
#include "db_ctl.h"

/*
	** error.c : 오류 발생 제어

	1) int fail_connect(void)
	** DB 연결 실패 시 호출되는 함수

	2) int fail_query(const char *query)
	** SQL 쿼리 실패 시 호출되는 함수

	오류 메시지를 출력하고, 재시도/종료 옵션을 제공한다.
	재시도 시 DB 연결 or SQL 쿼리 함수를 인자값과 함께 재호출하며,
	성공할 경우 0을 반환, 실패한 경우 다시 이 함수를 호출하게 된다.

*/

int fail_connect(void) {
	int key = 0;
	system("cls");
	printf("** 서버에 연결할 수 없습니다.\n");
	printf("** 인터넷에 연결되어 있는지 확인하십시오.\n");
	fprintf(stderr, "\n** Mysql connection error: %s\n", mysql_error(&db.conn));

	gotoxy(0, HEIGHT_SIZE - 1);
	printf("R : 재시도");
	gotoxy(WIDTH_SIZE - 10, HEIGHT_SIZE - 1);
	printf("ESC : 종료");

	
	while (1) {
		if (_kbhit()) {
			key = _getch();

			switch (key) {
			case KEY_R: //재시도
				system("cls");
				DB_CONNECT();
				return 0;
				break;
			case KEY_ESC: //종료
				system("cls");
				return 1;
				break;
			default:
				break;
			}
		}
		Sleep(100);
	}
}

int fail_query(const char *query) {
	int key = 0;
	system("cls");
	printf("** 쿼리 중 오류가 발생했습니다.\n");
	fprintf(stderr, "\n** Mysql query error: %s\n", mysql_error(&db.conn));

	gotoxy(0, HEIGHT_SIZE - 1);
	printf("R : 재시도");
	gotoxy(WIDTH_SIZE - 10, HEIGHT_SIZE - 1);
	printf("ESC : 종료");


	while (1) {
		if (_kbhit()) {
			key = _getch();

			switch (key) {
			case KEY_R:
				system("cls");
				SQL_QUERY(query);
				return 0;
				break;
			case KEY_ESC:
				system("cls");
				return 1;
				break;
			default:
				break;
			}
		}
		Sleep(100);
	}
}