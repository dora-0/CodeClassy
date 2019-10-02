/*
#include <stdio.h>
#include <conio.h>
#include "main.h"
#include "db_ctl.h"

int getItemInfo(void) {
	struct _item *item[MAX_ITEM];
	int i = 0;
	int num_row = 0;

	system("cls");
	DB_CONNECT();
	SQL_QUERY("select * from items");
	//num_row = mysql_num_rows(db.sql_result); //bug
	num_row = 2;

	while ((db.sql_row = mysql_fetch_row(db.sql_result)) != NULL) {
		item[i] = (struct _item *)malloc(sizeof(struct _item));
		item[i]->id = atoi(db.sql_row[0]);
		item[i]->name = db.sql_row[1];
		item[i]->type = db.sql_row[2];
		item[i]->rare = db.sql_row[3];
		item[i]->max_level = atoi(db.sql_row[4]);
		item[i]->p_attack = atoi(db.sql_row[5]);
		item[i]->p_defense = atoi(db.sql_row[6]);
		item[i]->m_attack = atoi(db.sql_row[7]);
		item[i]->m_defense = atoi(db.sql_row[8]);
		i++;
	}

	for (i = 0; i < num_row; i++) {
		printf("====================\n");
		printf("Item ID: %d\n", item[i]->id);
		printf("이름: %s\n", item[i]->name);
		printf("타입: %s\n", item[i]->type);
		printf("레어도: %s\n", item[i]->rare);
		printf("최대 레벨: %d\n", item[i]->max_level);
		printf("물리 공격력: %d\n", item[i]->p_attack);
		printf("물리 방어력: %d\n", item[i]->p_defense);
		printf("마법 공격력: %d\n", item[i]->m_attack);
		printf("마법 방어력: %d\n", item[i]->m_defense);
	}
	
	db_disconnect();
	_getch();
	return 0;
}
*/