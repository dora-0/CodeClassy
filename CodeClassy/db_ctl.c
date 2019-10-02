#include <stdio.h>
#include <string.h>
#include "main.h"
#include "db_ctl.h"

/*
	** db.ctl.c : MySQL ���̺귯���� ����� DB ����

	MySQL ����ü (db_ctl.h ����)
	struct db_conn {
		MYSQL conn;
		MYSQL *connection;
		MYSQL_RES *sql_result;
		MYSQL_ROW sql_row;
	}db;

	1) int db_connect(void)
	** DB ���� �� ����ϴ� �Լ�

	#define DB_CONNECT(): ��ũ�η� ���ǵ� �Լ�
	�� if (db_connect() != 0) exit(1)

	db_connect() �Լ� ���� �� ��ȯ���� 0�� �ƴϸ�, ���α׷��� ����

	DB_CONNECT() ��� �� error.c�� fail_connect() �Լ��� ����Ͽ�
	��ȯ���� �ٷ� ���ϵ��� �ʵ��� ��.
	fail_connect()���� ��õ�, �ٷ� ���� �ɼ��� �����ϰ�,
	��õ� �� db_connect()�� ��ȣ���Ͽ� ���ῡ ������ ��� ������ ��ȯ���� �����ϵ��� ó����.

	===

	2) void db_disconnect(void)
	** DB ���� ���� �� ����ϴ� �Լ�
	DB ���� ���¸� �����ϸ� connect_notifier(0) �Լ��� ���� ����ȴ�.

	===

	3) int sql_query(const char *query)
	** SQL ���� �� ����ϴ� �Լ�

	#define SQL_QUERY(x): ��ũ�η� ���ǵ� �Լ�
	�� if (sql_query(x) != 0) exit(1)

	sql_query() �Լ� ���� �� ��ȯ���� 0�� �ƴϸ�, ���α׷��� ����

	DB_CONNECT() ��� �� error.c�� fail_query() �Լ��� ����Ͽ�
	��ȯ���� �ٷ� ���ϵ��� �ʵ��� ��.
	fail_query()���� ��õ�, �ٷ� ���� �ɼ��� �����ϰ�,
	��õ� �� sql_query()�� ���ڰ��� �Բ� ��ȣ���Ͽ� ���ῡ ������ ��� ������ ��ȯ���� �����ϵ��� ó����.

*/
int db_connect(void) {
	db.connection = NULL;
	connect_notifier(1); //ȭ�� ���� �ϴ��� [���� ��] ǥ��
	mysql_init(&db.conn);

	db.connection = mysql_real_connect(&db.conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, DB_PORT, (char *)NULL, 0);
	//������ ���������� �̷������ �ʾ��� ���
	if (db.connection == NULL) {
		connect_notifier(0);
		return fail_connect();
	}
	else {
		//printf("\n** Connect OK\n** Host: %s:%d\n** Username: %s\n** Database Name: %s\n", DB_HOST, DB_PORT, DB_USER, DB_NAME);
	}

	mysql_query(db.connection, "set names euckr"); //�ѱ� ���� ������
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