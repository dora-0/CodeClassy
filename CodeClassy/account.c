#include <stdio.h>
#include <string.h>
#include "db_ctl.h"
#include "main.h"
#define SMALL_ARRSIZE 128
/*
	** account.c : 로그인/회원가입 제어 (DB 기능 응용)

	MySQL 구조체 (db_ctl.h 선언)
	struct db_conn {
		MYSQL conn;
		MYSQL *connection;
		MYSQL_RES *sql_result;
		MYSQL_ROW sql_row;
	}db;

	1) int FlushArray(char *arr, const int ArraySize)
	** 배열의 내용을 비워주는 함수

	첫번째 인자값으로 배열의 주소, 두번째 인자값으로 배열의 크기를 받는다.
	데이터를 모두 '\0'으로 덮어씌운다.

	===

	2) int InputID(char *userID)
	** 로그인/회원가입 시 ID를 입력받는 함수

	scanf 대신 _getch를 응용해 영문 대소문자만 입력받도록 하고,
	백스페이스 키를 누르면 지워지도록 한다.

	===

	3) int InputPW(char *password)
	** 로그인/회원가입 시 패스워드를 입력받는 함수

	InputID() 함수와 비슷하나 문자 입력 시 문자가 그대로 보이지 않고 '*'로 마스킹 처리되어 표시됨.

	===

	4) int joinUser(void)
	** 회원가입 처리 함수

	아이디 + 비밀번호를 입력받고 회원가입을 진행한다.
	비밀번호는 2번 입력받는데, 2번 입력받은 값이 서로 다르면 패스워드가 일치하지 않다는 경고를 띄우고
	다시 입력하게 함.
	이후 닉네임을 설정하고 서버로 데이터를 전송한다.
	(패스워드의 경우 MySQL에서 지원하는 PASSWORD() 함수로 암호화 처리되어 전송된다.)

	===

	5) int Login(void)
	** 로그인 처리 함수
	
	아이디 + 비밀번호를 입력받고 로그인을 진행한다.
	입력받은 아이디와 비밀번호를 서버의 데이터와 비교해 서로 일치한다면 계졍 고유의 account_id 값을 부여받는다.
	해당 값으로 서버에 저장되어 있는 유저 데이터에 액세스할 수 있게 된다.

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
	printf("\n데이터를 저장할 계정을 생성합니다.\n");
	//printf("닉네임을 입력해주세요 (게임 내에서 사용됨) : \n");
	//scanf("%20[^\n]", user.username);
	printf("\n아이디 입력 (최대 20자) : ");
	InputID(userID);
	
	while (1) {
		FlushArray(password1, SMALL_ARRSIZE);
		FlushArray(password2 ,SMALL_ARRSIZE);

		printf("\n비밀번호 입력 (최대 20자 / 영문 대소문자, 숫자만 가능) : \n");
		InputPW(password1);
		printf("\n비밀번호 재입력 : \n");
		InputPW(password2);

		if (SAME(password1, password2))
			break;
		else {
			system("cls");
			setFontColor(CC_RED);
			printf("\n오류 : 비밀번호가 일치하지 않습니다. 다시 입력해주세요.\n\n");
			setFontColor(CC_WHITE);
		}
	}

	while (1) {
		system("cls");
		printf("\n닉네임을 설정해 주세요\n(한글 최대 10자, 영문 최대 20자까지 가능)\n : ");
		scanf("%20[^\n]", user.username);
		while (getchar() != '\n');
		system("cls");
		printf("\n%s로 설정하시겠습니까? (Y/N) : ", user.username);
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
	printf("\n기존 계정으로 로그인합니다.\n");

	while (1) {
		FlushArray(userID, SMALL_ARRSIZE);
		FlushArray(password, SMALL_ARRSIZE);

		printf("\n아이디 입력 : ");
		InputID(userID);
		printf("\n비밀번호 입력 : ");
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
			printf("\n오류 : 아이디 또는 비밀번호가 일치하지 않습니다. 다시 입력해주세요.\n\n");
			setFontColor(CC_WHITE);
			db_disconnect();
		}
	}

	gamectl(GAME_INIT);
	return 0;
}