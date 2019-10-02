#include <stdio.h>
#include "main.h"
#include "db_ctl.h"

/*
	** init.c : 시스템 초기 설정

	1) void init(void)
	** 콘솔 창 제목, 크기 등을 정의하는 초기화 함수

	게임의 시작점에서 핵심 부분의 초기화를 담당한다.
	CursorView(0) : 콘솔 창 커서가 보이지 않도록 함.
	InitConsole() : 사용자가 창 최대화 및 크기 조정을 사용할 수 없도록 하고, 빠른 편집 모드를 비활성화해 선택 모드 전환으로 인한 게임 멈춤을 방지함.
	설정을 마친 후, DB 연결 테스트를 수행한다.

*/

void init(void) {
	char sys_cmd[200];
	system("title CodeClassy");
	sprintf(sys_cmd, "mode con: cols=%d lines=%d && color F", WIDTH_SIZE, HEIGHT_SIZE);
	system(sys_cmd);
	InitConsole();
	CursorView(0);
	//ChangeFontSize();

	DB_CONNECT();
	db_disconnect();
	while (menu_init());
	return;
}