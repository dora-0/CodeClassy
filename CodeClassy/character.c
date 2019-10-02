#include <stdio.h>
#include "main.h"
#include "db_ctl.h"
/*
	** character.c : ĳ���� ���� ����

	1) int get_max_level(void)
	** �ִ� ������ �����κ��� �������� �Լ�
	�����κ��� ���� �� �ʿ� ����ġ, �ش� ���� �ִ� HP, �ִ� TP,
	���� ���ݷ�, ���� ���� �����͸� ������ user ����ü ���� USER_LEVEL Ÿ�� user_level �迭�� ����.

	���� ������ ���� ������ �ִ� ������ �����ϰ� �ִ� ���� ���� ��ȯ.

	===

	2) void show_status_init(int hp_pos_X, int hp_pos_Y, int tp_pos_X, int tp_pos_Y)
	3) void show_status(int show_lvl_name, int is_grayscale_mode)
	** ����â ���� �Լ�

	�÷��̾� ����, �÷��̾� �̸�, HP ������, TP ������ ���� ����Ѵ�.
	�Լ� ��� �� show_status_init()���� ���� �ʿ��� ������ �̸� �����ؾ� �Ѵ�.

	-- show_status_init()
	hp_pos_X : HP �������� X ��ǥ
	hp_pos_Y : HP �������� Y ��ǥ
	tp_pos_X : TP �������� X ��ǥ
	tp_pos_Y : TP �������� Y ��ǥ

	-- show_status()
	show_lvl_name : �÷��̾� ���� �� �̸� ǥ�� ���� (ǥ�� ���� ��, HP ������ �ٷ� ���ʿ� ǥ�õ�)
	is_grayscale_mode : �׷��̽����� ��� ���� (�ִϸ��̼� ���� ����)

	===

	4) void show_character_info(void)
	** ĳ���� ���� ǥ��

	����ü _CurrentUser���� ���ǵ� ����ü ���� user�κ��� ���� ���� �ҷ��� ��� (->MySQL DB)

*/

//���� ���� ���� - show_status_init()�� ���� �ܺο��� ���� ����
static int stbar_hp_pos_X;
static int stbar_tp_pos_X;
static int stbar_hp_pos_Y;
static int stbar_tp_pos_Y;

//battle.c���� ������ ��
static int max_level;

int get_max_level(void) {
	int i = 0;
	DB_CONNECT();
	SQL_QUERY("select * from user_levels");
	while ((db.sql_row = mysql_fetch_row(db.sql_result)) != NULL) {
		user.user_level[i].level = atoi(db.sql_row[0]);
		user.user_level[i].required_exp = atoi(db.sql_row[1]);
		user.user_level[i].max_hp = atoi(db.sql_row[2]);
		user.user_level[i].max_tp = atoi(db.sql_row[3]);
		user.user_level[i].p_attack = atoi(db.sql_row[4]);
		user.user_level[i].p_defense = atoi(db.sql_row[5]);
		i++;
	}

	max_level = i; //������ ���� ���� = �ִ� ����
	db_disconnect();
	return max_level;
}

void show_status_init(int hp_pos_X, int hp_pos_Y, int tp_pos_X, int tp_pos_Y)
{
	stbar_hp_pos_X = hp_pos_X;
	stbar_hp_pos_Y = hp_pos_Y;
	stbar_tp_pos_X = tp_pos_X;
	stbar_tp_pos_Y = tp_pos_Y;
}

void show_status(int show_lvl_name, int is_grayscale_mode) {
	int j = 0;
	int hp_ratio = 0;
	int tp_ratio = 0;
	//HP, TP ������ �⺻ ����
	int hp_bg_color = CC_GREEN;
	int tp_bg_color = CC_CYAN;

	//�׷��̽����� ����� ��� ��ο� ȸ������ ����
	if (is_grayscale_mode) {
		hp_bg_color = CC_DARKGRAY;
		tp_bg_color = CC_DARKGRAY;
	}

	//HP ���� ��� (HP, TP ������ ��ȭ�� ���� ���)
	//�ִ� HP�� ������ ��� 100%
	if (user.hp == user.max_hp) {
		hp_ratio = 10;
	}
	else if (user.hp == 0) {
		hp_ratio = -1;
	}
	else {
		//hp_ratio ���� 1�� ���̸鼭, �÷��̾� �ִ� HP�� 10% * hp_ratio���� ���� HP�� �� ū ��� for�� ����
		for (hp_ratio = 9; hp_ratio > 0; hp_ratio--) {
			if (user.hp >= user.max_hp * (double)hp_ratio * 0.1) {
				break;
			}
		}
	}

	//TP ���� ���
	//�ִ� HP�� ������ ��� 100%
	if (user.tp == user.max_tp) {
		tp_ratio = 10;
	}
	else if (user.tp == 0) {
		tp_ratio = -1;
	}
	else {
		//tp_ratio ���� 1�� ���̸鼭, �÷��̾� �ִ� TP�� 10% * tp_ratio���� ���� TP�� �� ū ��� for�� ����
		for (tp_ratio = 9; tp_ratio > 0; tp_ratio--) {
			if (user.tp >= user.max_tp * (double)tp_ratio * 0.1) {
				break;
			}
		}
	}

	//level+username
	if (show_lvl_name == 1) {
		gotoxy(stbar_hp_pos_X, stbar_hp_pos_Y - 1);
		printf("Lv. %d %s", user.level, user.username);
	}

	//hp
	gotoxy(stbar_hp_pos_X, stbar_hp_pos_Y);
	printf("HP ");
	//BG ���� ���� �� hp_ratio ��ŭ printf�� ����
	setBgColor(hp_bg_color);
	for (j = 0; j <= hp_ratio; j++) {
		printf(" ");
	}

	//���̾ƿ��� �����ؾ� �ϹǷ� BG ������ ����Ʈ ������ ���� �� 10 - hp_ratio ��ŭ printf�� ����
	resetBgColor();
	for (j = 0; j <= 10 - hp_ratio; j++) {
		printf(" ");
	}

	printf("%3d/%d", user.hp, user.max_hp);

	//tp
	gotoxy(stbar_tp_pos_X, stbar_tp_pos_Y);
	printf("TP ");
	//BG ���� ���� �� hp_ratio ��ŭ printf�� ����
	setBgColor(tp_bg_color);
	for (j = 0; j <= tp_ratio; j++) {
		printf(" ");
	}

	//���̾ƿ��� �����ؾ� �ϹǷ� BG ������ ����Ʈ ������ ���� �� 10 - tp_ratio ��ŭ printf�� ����
	resetBgColor();
	for (j = 0; j <= 10 - tp_ratio; j++) {
		printf(" ");
	}

	printf("%3d/%d", user.tp, user.max_tp);

	//_getch();
}

void show_character_info(void) {
	int x = 50, y = 2;
	int max_level = get_max_level();
	gotoxy(x, y);
	printf("======ĳ���� ����======\n");
	gotoxy(x, y + 1);
	printf("���� ���ݷ�: %10d", user.p_attack);
	gotoxy(x, y + 2);
	printf("���� ����: %10d", user.p_defense);
	gotoxy(x, y + 3);
	printf("���� ���ݷ�: %10d", user.m_attack);
	gotoxy(x, y + 4);
	printf("���� ����: %10d", user.m_defense);
	gotoxy(x, y + 5);
	if (user.level == max_level)
		printf("     ����ġ: %4d/%5s", user.user_exp, "-");
	else
		printf("     ����ġ: %4d/%5d", user.user_exp, user.user_level[user.level].required_exp);
}