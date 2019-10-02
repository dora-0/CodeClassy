#include <stdio.h>
#include <stdlib.h>
#include "db_ctl.h"
#include "main.h"
#include "sound.h"

/*
	** game.c : 게임 시스템 제어

	1) int gamectl(int cmd)
	** 전체적인 게임 흐름을 제어하는 함수

	게임의 제어를 담당한다.
	게임 초기화, 시작, 세이브의 기능을 수행한다.
	main.h -> enum gamectl_commands에 정의된 값을 사용할 수 있다.

*/

int gamectl(int cmd) {
	char buf[MAX_ARRAY_SIZE];
	int ref_speed = get_ref_speed();

	if (cmd == GAME_INIT) { //게임 데이터를 서버로부터 불러옴
		DB_CONNECT();
		sprintf(buf, "select * from users where account_id='%d'", user.account_id);
		SQL_QUERY(buf);
		db.sql_row = mysql_fetch_row(db.sql_result);

		if (db.sql_row != NULL) {
			sprintf(user.username, db.sql_row[1]); //Username
			user.level = atoi(db.sql_row[2]);
			user.user_exp = atoi(db.sql_row[3]);
			user.hp = atoi(db.sql_row[4]);
			user.max_hp = atoi(db.sql_row[5]);
			user.tp = atoi(db.sql_row[6]);
			user.max_tp = atoi(db.sql_row[7]);
			user.p_attack = atoi(db.sql_row[8]);
			user.p_defense = atoi(db.sql_row[9]);
			user.m_attack = atoi(db.sql_row[10]);
			user.m_defense = atoi(db.sql_row[11]);

			user.map_id = atoi(db.sql_row[12]);
			user.map_x = atoi(db.sql_row[13]);
			user.map_y = atoi(db.sql_row[14]);
			user.gold = atoi(db.sql_row[15]);
		}
	}
	else if (cmd == GAME_START) { //게임 시작 시 호출 부분
		system("cls");
		show_status_init(1, HEIGHT_SIZE - 2, 26, HEIGHT_SIZE - 2);
		show_status(0, 0);
		xprint_init(3, HEIGHT_SIZE - 10, WIDTH_SIZE - 3, 8);
		xprint("먼 옛날, 마기문명이라는 고대의 문명이 있었다.\n그 문명은 찬란한 번영을 이뤘으나,\n어느 순간 아무도 모르게 사라지고 말았다.", 30, 1);
		xprint("수백년이 지난 후, 인류와 마족들이 대륙의 주도권을 잡게 되었다.\n두 종족은 타협하지 못한 채 서로 싸움을 계속했다.", 30, 1);
		xprint("950년 인류와 악마는 큰 전쟁을 벌였고,\n전쟁은 인류의 승리로 끝났다.", 30, 1);
		xprint("왕국의 통치하에 세계는 평화를 얻은 듯하였다..만...", 30, 1);
		xprint("어느 순간, 모험가들 사이로부터 한 가지 소문이 돌기 시작하였다.", 30, 1);
		xprint("바로 대륙의 중심부 쪽에,\n끝조차 보이지 않는 거대한 탑이 나타났다는 것.", 30, 1);
		xprint("탑의 꼭대기에는 나라의 모든 보물을 합친 것보다\n더 많은 보물이 있다거나 하는 소문이 돌았으나,\n이 탑에는 강력한 몬스터들이\n거주한다는 사실이 밝혀지면서 선뜻 나서는 이가 없었다.", 30, 1);
		xprint("그러던 어느 날, 한 용사가 이 탑의 꼭대기에 올라보리라는 결심을 하게 된다.", 30, 1);

		play_BGM("BGM\\home.ogg", FMOD_LOOP_NORMAL);

		while (home());

		play_BGM("BGM\\menu.ogg", FMOD_LOOP_NORMAL);
	}
	else if (cmd == GAME_SAVE) { //게임 데이터를 서버에 저장
		DB_CONNECT();
		sprintf(buf, "update users set username='%s', level='%d', user_exp='%d', hp='%d', max_hp='%d', tp='%d', max_tp='%d', p_attack='%d', p_defense='%d', m_attack='%d', m_defense='%d', map_id=0, map_x=1, map_y=1, gold='%d' where account_id='%d'", user.username, user.level, user.user_exp, user.hp, user.max_hp, user.tp, user.max_tp, user.p_attack, user.p_defense, user.m_attack, user.m_defense, user.gold, user.account_id);
		SQL_QUERY(buf);
		db_disconnect();
	}
	return 0;
}