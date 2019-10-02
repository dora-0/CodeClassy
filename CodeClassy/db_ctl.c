#include <stdio.h>
#include <string.h>
#include "main.h"
#include "db_ctl.h"

/*
	** db.ctl.c : MySQL 라이브러리를 사용한 DB 제어

	MySQL 구조체 (db_ctl.h 선언)
	struct db_conn {
		MYSQL conn;
		MYSQL *connection;
		MYSQL_RES *sql_result;
		MYSQL_ROW sql_row;
	}db;

	1) int db_connect(void)
	** DB 연결 시 사용하는 함수

	#define DB_CONNECT(): 매크로로 정의된 함수
	→ if (db_connect() != 0) exit(1)

	db_connect() 함수 실행 후 반환값이 0이 아니면, 프로그램을 종료

	DB_CONNECT() 사용 시 error.c의 fail_connect() 함수를 사용하여
	반환값이 바로 리턴되지 않도록 함.
	fail_connect()에서 재시도, 바로 종료 옵션을 제공하고,
	재시도 시 db_connect()를 재호출하여 연결에 성공한 경우 성공한 반환값을 리턴하도록 처리함.

	===

	2) void db_disconnect(void)
	** DB 연결 해제 시 사용하는 함수
	DB 연결 상태를 해제하며 connect_notifier(0) 함수도 같이 실행된다.

	===

	3) int sql_query(const char *query)
	** SQL 쿼리 시 사용하는 함수

	#define SQL_QUERY(x): 매크로로 정의된 함수
	→ if (sql_query(x) != 0) exit(1)

	sql_query() 함수 실행 후 반환값이 0이 아니면, 프로그램을 종료

	DB_CONNECT() 사용 시 error.c의 fail_query() 함수를 사용하여
	반환값이 바로 리턴되지 않도록 함.
	fail_query()에서 재시도, 바로 종료 옵션을 제공하고,
	재시도 시 sql_query()를 인자값과 함께 재호출하여 연결에 성공한 경우 성공한 반환값을 리턴하도록 처리함.

*/
int db_connect(void) {
	db.connection = NULL;
	connect_notifier(1); //화면 우측 하단의 [연결 중] 표시
	mysql_init(&db.conn);

	db.connection = mysql_real_connect(&db.conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, DB_PORT, (char *)NULL, 0);
	//연결이 정상적으로 이루어지지 않았을 경우
	if (db.connection == NULL) {
		connect_notifier(0);
		return fail_connect();
	}
	else {
		//printf("\n** Connect OK\n** Host: %s:%d\n** Username: %s\n** Database Name: %s\n", DB_HOST, DB_PORT, DB_USER, DB_NAME);
	}

	mysql_query(db.connection, "set names euckr"); //한글 깨짐 방지용
	return 0;
}

void db_disconnect(void) {
	mysql_free_result(db.sql_result);
	mysql_close(db.connection);
	connect_notifier(0);
	return;
}

int sql_query(const char *query) {
	int query_stat;
	db.sql_result = NULL;
	db.sql_row = NULL;

	query_stat = mysql_query(db.connection, query);

	if (query_stat != 0) {
		connect_notifier(0);
		return fail_query(query);
	}

	db.sql_result = mysql_store_result(db.connection);
	//printf("** Query OK\n");
	return 0;
}