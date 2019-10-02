#include <stdio.h>
#include <conio.h>
#include "main.h"
#include "db_ctl.h"

/*
	** error.c : ���� �߻� ����

	1) int fail_connect(void)
	** DB ���� ���� �� ȣ��Ǵ� �Լ�

	2) int fail_query(const char *query)
	** SQL ���� ���� �� ȣ��Ǵ� �Լ�

	���� �޽����� ����ϰ�, ��õ�/���� �ɼ��� �����Ѵ�.
	��õ� �� DB ���� or SQL ���� �Լ��� ���ڰ��� �Բ� ��ȣ���ϸ�,
	������ ��� 0�� ��ȯ, ������ ��� �ٽ� �� �Լ��� ȣ���ϰ� �ȴ�.

*/

int fail_connect(void) {
	int key = 0;
	system("cls");
	printf("** ������ ������ �� �����ϴ�.\n");
	printf("** ���ͳݿ� ����Ǿ� �ִ��� Ȯ���Ͻʽÿ�.\n");
	fprintf(stderr, "\n** Mysql connection error: %s\n", mysql_error(&db.conn));

	gotoxy(0, HEIGHT_SIZE - 1);
	printf("R : ��õ�");
	gotoxy(WIDTH_SIZE - 10, HEIGHT_SIZE - 1);
	printf("ESC : ����");

	
	while (1) {
		if (_kbhit()) {
			key = _getch();

			switch (key) {
			case KEY_R: //��õ�
				system("cls");
				DB_CONNECT();
				return 0;
				break;
			case KEY_ESC: //����
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
	printf("** ���� �� ������ �߻��߽��ϴ�.\n");
	fprintf(stderr, "\n** Mysql query error: %s\n", mysql_error(&db.conn));

	gotoxy(0, HEIGHT_SIZE - 1);
	printf("R : ��õ�");
	gotoxy(WIDTH_SIZE - 10, HEIGHT_SIZE - 1);
	printf("ESC : ����");


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