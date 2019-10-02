#include <stdio.h>
#include "main.h"
#include "db_ctl.h"

/*
	** init.c : �ý��� �ʱ� ����

	1) void init(void)
	** �ܼ� â ����, ũ�� ���� �����ϴ� �ʱ�ȭ �Լ�

	������ ���������� �ٽ� �κ��� �ʱ�ȭ�� ����Ѵ�.
	CursorView(0) : �ܼ� â Ŀ���� ������ �ʵ��� ��.
	InitConsole() : ����ڰ� â �ִ�ȭ �� ũ�� ������ ����� �� ������ �ϰ�, ���� ���� ��带 ��Ȱ��ȭ�� ���� ��� ��ȯ���� ���� ���� ������ ������.
	������ ��ģ ��, DB ���� �׽�Ʈ�� �����Ѵ�.

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