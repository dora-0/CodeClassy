#pragma once
#pragma warning (disable:4996)
#include <mysql.h>
#define DB_HOST "192.168.123.123"
#define DB_PORT 33060
#define DB_USER "classy"
#define DB_PASS "TCDPbymFSKBhXJaz"
#define DB_NAME "classy_test"
#define DB_CONNECT() if (db_connect() != 0) exit(1)
#define SQL_QUERY(x) if (sql_query(x) != 0) exit(1)

int db_connect(void);
void db_disconnect(void);
int sql_query(const char *query);

struct db_conn {
	MYSQL conn;
	MYSQL *connection;
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
}db;