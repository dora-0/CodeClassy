#include <stdio.h>
#include <string.h>
#include "db_ctl.h"
#include "main.h"
#define SMALL_ARRSIZE 128
/*
	** account.c : �α���/ȸ������ ���� (DB ��� ����)

	MySQL ����ü (db_ctl.h ����)
	struct db_conn {
		MYSQL conn;
		MYSQL *connection;
		MYSQL_RES *sql_result;
		MYSQL_ROW sql_row;
	}db;

	1) int FlushArray(char *arr, const int ArraySize)
	** �迭�� ������ ����ִ� �Լ�

	ù��° ���ڰ����� �迭�� �ּ�, �ι�° ���ڰ����� �迭�� ũ�⸦ �޴´�.
	�����͸� ��� '\0'���� ������.

	===

	2) int InputID(char *userID)
	** �α���/ȸ������ �� ID�� �Է¹޴� �Լ�

	scanf ��� _getch�� ������ ���� ��ҹ��ڸ� �Է¹޵��� �ϰ�,
	�齺���̽� Ű�� ������ ���������� �Ѵ�.

	===

	3) int InputPW(char *password)
	** �α���/ȸ������ �� �н����带 �Է¹޴� �Լ�

	InputID() �Լ��� ����ϳ� ���� �Է� �� ���ڰ� �״�� ������ �ʰ� '*'�� ����ŷ ó���Ǿ� ǥ�õ�.

	===

	4) int joinUser(void)
	** ȸ������ ó�� �Լ�

	���̵� + ��й�ȣ�� �Է¹ް� ȸ�������� �����Ѵ�.
	��й�ȣ�� 2�� �Է¹޴µ�, 2�� �Է¹��� ���� ���� �ٸ��� �н����尡 ��ġ���� �ʴٴ� ��� ����
	�ٽ� �Է��ϰ� ��.
	���� �г����� �����ϰ� ������ �����͸� �����Ѵ�.
	(�н������� ��� MySQL���� �����ϴ� PASSWORD() �Լ��� ��ȣȭ ó���Ǿ� ���۵ȴ�.)

	===

	5) int Login(void)
	** �α��� ó�� �Լ�
	
	���̵� + ��й�ȣ�� �Է¹ް� �α����� �����Ѵ�.
	�Է¹��� ���̵�� ��й�ȣ�� ������ �����Ϳ� ���� ���� ��ġ�Ѵٸ� ���� ������ account_id ���� �ο��޴´�.
	�ش� ������ ������ ����Ǿ� �ִ� ���� �����Ϳ� �׼����� �� �ְ� �ȴ�.

*/
int FlushArray(char *arr, const int ArraySize) {
	int i = 0;
	for (i = 0; i < ArraySize; i++) {
		arr[i] = '\0';
	}

	return 0;
}

int InputID(char *userID) {
	int i = 0;
	while (1) {
		char key = _getch();
		if (key == KEY_ENTER)
			break;
		else if (key == KEY_BSP) {
			if (i > 0) {
				userID[i] = '\0';
				printf("\b \b");
				i--;
			}
		}
		else if (i < 20) {
			if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z')) {
				userID[i] = key;
				printf("%c", userID[i]);
				i++;
			}
		}

	}

	return 0;
}

int InputPW(char *password) {
	int i = 0;
	while (1) {
		char key = _getch();
		if (key == KEY_ENTER)
			break;
		else if (key == KEY_BSP) {
			if (i > 0) {
				password[i] = '\0';
				printf("\b \b");
				i--;
			}
		}
		else if (i < 20) {
			if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z')) {
				password[i] = key;
				printf("*");
				i++;
			}
		}
		
	}

	//printf("\nPASSWORD: \"%s\"\n", password);
	//system("pause");
	return 0;
}

int joinUser(void) {
	char userID[SMALL_ARRSIZE] = { 0 };
	char password1[SMALL_ARRSIZE] = { 0 };
	char password2[SMALL_ARRSIZE] = { 0 };
	char buf[MAX_ARRAY_SIZE];
	char temp;

	system("cls");
	printf("\n�����͸� ������ ������ �����մϴ�.\n");
	//printf("�г����� �Է����ּ��� (���� ������ ����) : \n");
	//scanf("%20[^\n]", user.username);
	printf("\n���̵� �Է� (�ִ� 20��) : ");
	InputID(userID);
	
	while (1) {
		FlushArray(password1, SMALL_ARRSIZE);
		FlushArray(password2 ,SMALL_ARRSIZE);

		printf("\n��й�ȣ �Է� (�ִ� 20�� / ���� ��ҹ���, ���ڸ� ����) : \n");
		InputPW(password1);
		printf("\n��й�ȣ ���Է� : \n");
		InputPW(password2);

		if (SAME(password1, password2))
			break;
		else {
			system("cls");
			setFontColor(CC_RED);
			printf("\n���� : ��й�ȣ�� ��ġ���� �ʽ��ϴ�. �ٽ� �Է����ּ���.\n\n");
			setFontColor(CC_WHITE);
		}
	}

	while (1) {
		system("cls");
		printf("\n�г����� ������ �ּ���\n(�ѱ� �ִ� 10��, ���� �ִ� 20�ڱ��� ����)\n : ");
		scanf("%20[^\n]", user.username);
		while (getchar() != '\n');
		system("cls");
		printf("\n%s�� �����Ͻðڽ��ϱ�? (Y/N) : ", user.username);
		scanf("%c", &temp);
		while (getchar() != '\n');

		if (temp == 'y' || temp == 'Y')
			break;
		else
			continue;
	}
	
	//DB
	DB_CONNECT();
	sprintf(buf, "insert into account (userID, password) values ('%s', PASSWORD('%s'))", userID, password1);
	SQL_QUERY(buf);
	FlushArray(buf, MAX_ARRAY_SIZE);
	mysql_free_result(db.sql_result);
	Sleep(100);

	sprintf(buf, "select * from account where userID='%s' AND password=PASSWORD('%s')", userID, password1);
	SQL_QUERY(buf);
	FlushArray(buf, MAX_ARRAY_SIZE);
	db.sql_row = mysql_fetch_row(db.sql_result);

	if (db.sql_row != NULL) {
		user.account_id = atoi(db.sql_row[0]);
	}

	mysql_free_result(db.sql_result);
	Sleep(100);

	sprintf(buf, "insert into users (account_id, username) values ('%d', '%s')", user.account_id, user.username);
	SQL_QUERY(buf);
	FlushArray(buf, MAX_ARRAY_SIZE);
	Sleep(100);

	gamectl(GAME_INIT);
	db_disconnect();
	return 0;
}

int Login(void) {
	char userID[SMALL_ARRSIZE] = { 0 };
	char password[SMALL_ARRSIZE] = { 0 };
	char buf[MAX_ARRAY_SIZE];

	system("cls");
	printf("\n���� �������� �α����մϴ�.\n");

	while (1) {
		FlushArray(userID, SMALL_ARRSIZE);
		FlushArray(password, SMALL_ARRSIZE);

		printf("\n���̵� �Է� : ");
		InputID(userID);
		printf("\n��й�ȣ �Է� : ");
		InputPW(password);

		//DB
		DB_CONNECT();
		sprintf(buf, "select * from account where userID='%s' AND password=PASSWORD('%s')", userID, password);
		SQL_QUERY(buf);
		db.sql_row = mysql_fetch_row(db.sql_result);

		if (db.sql_row != NULL) {
			user.account_id = atoi(db.sql_row[0]);
			db_disconnect();
			break;
		}
		else {
			system("cls");
			setFontColor(CC_RED);
			printf("\n���� : ���̵� �Ǵ� ��й�ȣ�� ��ġ���� �ʽ��ϴ�. �ٽ� �Է����ּ���.\n\n");
			setFontColor(CC_WHITE);
			db_disconnect();
		}
	}

	gamectl(GAME_INIT);
	return 0;
}