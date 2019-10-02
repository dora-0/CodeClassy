#include <stdio.h>
#include <stdlib.h>
#include "db_ctl.h"
#include "main.h"
#include "sound.h"

/*
	** game.c : ���� �ý��� ����

	1) int gamectl(int cmd)
	** ��ü���� ���� �帧�� �����ϴ� �Լ�

	������ ��� ����Ѵ�.
	���� �ʱ�ȭ, ����, ���̺��� ����� �����Ѵ�.
	main.h -> enum gamectl_commands�� ���ǵ� ���� ����� �� �ִ�.

*/

int gamectl(int cmd) {
	char buf[MAX_ARRAY_SIZE];
	int ref_speed = get_ref_speed();

	if (cmd == GAME_INIT) { //���� �����͸� �����κ��� �ҷ���
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
	else if (cmd == GAME_START) { //���� ���� �� ȣ�� �κ�
		system("cls");
		show_status_init(1, HEIGHT_SIZE - 2, 26, HEIGHT_SIZE - 2);
		show_status(0, 0);
		xprint_init(3, HEIGHT_SIZE - 10, WIDTH_SIZE - 3, 8);
		xprint("�� ����, ���⹮���̶�� ����� ������ �־���.\n�� ������ ������ ������ �̷�����,\n��� ���� �ƹ��� �𸣰� ������� ���Ҵ�.", 30, 1);
		xprint("������� ���� ��, �η��� �������� ����� �ֵ����� ��� �Ǿ���.\n�� ������ Ÿ������ ���� ä ���� �ο��� ����ߴ�.", 30, 1);
		xprint("950�� �η��� �Ǹ��� ū ������ ������,\n������ �η��� �¸��� ������.", 30, 1);
		xprint("�ձ��� ��ġ�Ͽ� ����� ��ȭ�� ���� ���Ͽ���..��...", 30, 1);
		xprint("��� ����, ���谡�� ���̷κ��� �� ���� �ҹ��� ���� �����Ͽ���.", 30, 1);
		xprint("�ٷ� ����� �߽ɺ� �ʿ�,\n������ ������ �ʴ� �Ŵ��� ž�� ��Ÿ���ٴ� ��.", 30, 1);
		xprint("ž�� ����⿡�� ������ ��� ������ ��ģ �ͺ���\n�� ���� ������ �ִٰų� �ϴ� �ҹ��� ��������,\n�� ž���� ������ ���͵���\n�����Ѵٴ� ����� �������鼭 ���� ������ �̰� ������.", 30, 1);
		xprint("�׷��� ��� ��, �� ��簡 �� ž�� ����⿡ �ö󺸸���� ����� �ϰ� �ȴ�.", 30, 1);

		play_BGM("BGM\\home.ogg", FMOD_LOOP_NORMAL);

		while (home());

		play_BGM("BGM\\menu.ogg", FMOD_LOOP_NORMAL);
	}
	else if (cmd == GAME_SAVE) { //���� �����͸� ������ ����
		DB_CONNECT();
		sprintf(buf, "update users set username='%s', level='%d', user_exp='%d', hp='%d', max_hp='%d', tp='%d', max_tp='%d', p_attack='%d', p_defense='%d', m_attack='%d', m_defense='%d', map_id=0, map_x=1, map_y=1, gold='%d' where account_id='%d'", user.username, user.level, user.user_exp, user.hp, user.max_hp, user.tp, user.max_tp, user.p_attack, user.p_defense, user.m_attack, user.m_defense, user.gold, user.account_id);
		SQL_QUERY(buf);
		db_disconnect();
	}
	return 0;
}