#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "sound.h"
static int ref_speed = 60; //while (keyInput(&cursor2, 0, enemyCount - 1))
static int alreadyEscapeClicked = 0;
extern CurrentEnemies *_enemy[3];
/*
	** battle.c : ���� ����

	1) struct _BattleInfo result
	** ���� �� �����͸� ������ ����ü

	���� ���� �� ȹ���� ����ġ, ��带 �ӽ÷� ��Ƶд�.
	
	===

	2) void ResetEscapeClickFlag(void)
	** ����ġ�� ��ư ���� �Լ�

	���� ���� ������ alreadyEscapeClicked�� ����ġ�⸦ 1ȸ�� �õ��� �� �ֵ��� �ϱ� ����
	����ġ�� ��ư�� �ԷµǸ� ���� 1�� ����ȴ�.
	��, ���ο� ������ ���۵Ǹ� �ٽ� ����ġ�⸦ ����� �� �ֵ��� �ؾ� �ϹǷ�
	�ش� �Լ��� ȣ���ϸ� ���� ���� 0���� �����Ų��.

	===

	3) int AttackProcess(int mode)
	** ���� ���μ���

	int mode :
	AP_PtE : Player to Enemy (���� ������ ��)
	AP_EtP : Enemy to Player (������ ���ݹ��� ��)

	�� ���� :
	�÷��̾� -> �� 1 -> �� 2 -> �� 3 -> �÷��̾� -> ... (�� ���� ���� ���� ������ ���� �ߵ�)

	===

	4) int hit_damage(int *target, int damage)
	** ������ �ǰ� ó�� �Լ�

	PtE, EtP �������� ��밡��.
	Ÿ������ ������ ��밡 ü���� 0�� �Ǿ����� ���θ� ��ȯ�Ѵ�. (return killed)

	===

	5) void tp_up(int damage)
	** TP ��� ó�� �Լ� (�÷��̾� ����)

	������ �ǰݹ����� ���� ��ġ��ŭ �÷��̾��� TP�� ��½�Ų��.
	������ (���� ������) / (�÷��̾��� �ִ� HP) * 100

	===

	6) void game_over(void)
	** ���� ���� �� ȣ��

	�÷��̾��� ü���� 0�� �Ǿ� �����Ҵ��� �� ��� ȣ���.

	�÷��̾� HP, TP �ʱ�ȭ + �̹� ���� �� ȹ���� EXP, ��� �ʱ�ȭ.

	===

	7) void battle_result(void)
	** ���� �¸� �� ȣ��

	�÷��̾ �� �������� ��� ���� �����߸��� ȣ���.
	�̹� ���� �� ȹ���� EXP, ��带 ���� �����Ϳ� �ݿ�.

	���� ���� �� üũ �Լ��� ȣ����.

	===

	8) void lvlup_check(void)
	** ���� �� üũ �Լ�

	������ user_levels ���̺��� �����͸� ������ ���� ���� �Ǿ�����(=�ʿ� EXP �̻��� �Ǿ�����) üũ.
	���� ���� �� ��� �÷��̾��� ���� �� ���� �ִ� HP, �ִ� TP, ���ݷ�, ���� ��ġ�� ������Ʈ �ϰ�
	���� ü���� Ǯ ȸ��.

	===

	9) void showEnemyList(void)
	** �� ���� ��� �Լ� (�ܼ� ��¿�)

	===

	9) int keyInput_selectEnemy(int *cursor, int min_pos, int max_pos)
	10) void select_enemy(int *cursor, int enemyCount)
	** ���� ��ư ���� �� �� ���� ó��

	���õ� ���� �÷���, ���õ��� ������ ȸ������ ǥ��.
	keyInput_selectEnemy : select_enemy �Լ��� �°� �Ϻ� ����� ������ keyInput() �Լ� (óġ�� �� ����)
	�Է¹��� ���� ���� Ŀ�� ���� Ű ���ð��� ���� ����

	===

	12) int battle_screen(void)
	** ���� ȭ�� ����

	���� �߻� �� ����� ȭ���� �����Ѵ�.
	���� / ��ų / ����ģ�� (=����)

*/

struct _BattleInfo {
	int get_exp;
	int gold;
}result = {.get_exp = 0, .gold = 0};

void ResetEscapeClickFlag(void) {
	alreadyEscapeClicked = 0;
}

int AttackProcess(int mode) {
	int i;
	int active_enemy;
	int battle_result_flag = -1;
	int damage;
	int enemy_killed = 0;
	int user_killed = 0;
	char buf[MAX_ARRAY_SIZE];

	for (i = 0; i < 3; i++) {
		if (_enemy[i]->active == 1) { //select_enemy() -> battle_screen()���� ���õ� �� ã��
			active_enemy = i;
			break;
		}
	}

	if (mode == AP_PtE) { //�÷��̾ ���� ������ ��
		damage = user.p_attack - _enemy[active_enemy]->p_defense; //���� ������: �÷��̾��� ���� ���ݷ� - ���õ� ���� ���� ����
		if (damage < 1) damage = 0;

		enemy_killed = hit_damage(&(_enemy[active_enemy]->hp), damage); //�ǰ� ó�� �Լ� ȣ�� (�� óġ ���� ��ȯ)

		showEnemyList();
		play_SE("SE\\att-sound1-9.ogg", FMOD_DEFAULT);
		sprintf(buf, "%s�� �������� %s���� %d�� �������� ������!", user.username, _enemy[active_enemy]->name, damage);
		xprint_init(2, 20, WIDTH_SIZE - 2, 4);
		xprint(buf, 30, 1);

		if (enemy_killed) { //���� óġ�� ���
			_enemy[active_enemy]->active = -1; //�� ��Ȱ��ȭ
			result.get_exp += 10 * (_enemy[active_enemy]->level); //ȹ�� ����ġ: ���õ� ���� ���� * 10
			result.gold += (_enemy[active_enemy]->p_attack * _enemy[active_enemy]->level); //ȹ�� ���: ���õ� ���� ���� ���ݷ� * ����
			sprintf(buf, "%s(��)�� ��������!", _enemy[active_enemy]->name);
			xprint_init(2, 21, 0, 0);
			xprint(buf, 30, 1);
			battle_result_flag = 1; //flag = 1�� �켱 ����

			for (i = 0; i < 3; i++) {
				if (_enemy[i]->active != -1) //���� ���� ���� ������ flag = 0
					battle_result_flag = 0;
			}

			if (battle_result_flag) { //���� ��� óġ�� ���
				battle_result(); //ȣ��
				return 0;
			}
		}
	}

	else if (mode == AP_EtP) { //�÷��̾ ������ ���ݹ��� ��
		int i;
		for (i = 0; i < 3; i++) { //�� 1~3������ ������� ����
			if (_enemy[i]->active != -1) { //��Ȱ��ȭ�� ���� ����
				xprint_init(2, 20, WIDTH_SIZE - 2, 4);
				damage = _enemy[i]->p_attack - user.p_defense; //���� ������: ���� ���� ���ݷ� - �÷��̾��� ���� ����
				if (damage < 1) damage = 0;

				user_killed = hit_damage(&(user.hp), damage); //�ǰ� ó�� �Լ� ȣ�� (�÷��̾� �����Ҵ� ���� ��ȯ)
				tp_up(damage); //TP ��� ó�� �Լ� ȣ��

				show_status(1, 0);
				sprintf(buf, "ENEMY %d: ���� ����! %s���� %d��ŭ�� �������� �Ծ���!", i + 1, _enemy[i]->name, damage);

				xprint(buf, 30, 1);
			}

			if (user_killed) { //�÷��̾ �����Ҵ��� �� ���
				//battle_result_flag = 1;
				game_over(); //���ӿ��� �Լ� ȣ��
				return 0;
			}
		}
	}

	//���õ� �� ����
	for (i = 0; i < 3; i++) {
		if (_enemy[i]->active == 1) {
			_enemy[i]->active = 0;
			break;
		}
	}

	return 1;
}

enum hit_damage_target {
	USER,
	ENEMY
};

int hit_damage(int *target, int damage) { //�ǰ� ó�� (Ÿ�� = Ÿ���� ���� HP ����, ������)
	int killed = 0;

	if ((*target - damage) <= 0) { //���� HP - �������� 0 �����̸� ��� ����
		*target = 0;
		killed = 1;
	}
	else {
		*target -= damage;
	}
	
	return killed; //��� ���� ��ȯ
}

void tp_up(int damage) { //TP ��� ó��
	user.tp += (int)(((double)damage / user.max_hp) * 100);
	if (user.tp > user.max_tp) user.tp = user.max_tp;
}

void game_over(void) { //���ӿ��� ȭ��
	xprint_init(2, 20, WIDTH_SIZE - 2, 4);
	setFontColor(CC_RED);
	xprint("�����Ҵ� ���°� �Ǿ����ϴ�.", 30, 1);
	setFontColor(CC_WHITE);
	user.hp = user.max_hp;
	user.tp = 0;
	result.get_exp = 0;
	result.gold = 0;
}

void battle_result(void) { //�����¸� ȭ��
	char buf[MAX_ARRAY_SIZE];

	user.user_exp += result.get_exp;
	user.gold += result.gold;
	drawMenuBorder(24, 8);
	sprintf(buf, "���� ����!\n\nEXP +%d\nGOLD +%d", result.get_exp, result.gold);
	result.get_exp = 0;
	result.gold = 0;
	xprint_init(27, 10, 0, 0);
	xprint(buf, 30, 1);
	lvlup_check();
}

void lvlup_check(void) { //���� �� üũ
	int i;
	int before_level = user.level;
	int after_level = user.level + 1;
	int lvlup_flag = 1;
	char buf[MAX_ARRAY_SIZE];

	int max_level = get_max_level();

	for (i = user.level; i < max_level; i++) { //Level 1�� ���, user.user.level[1] -> �� Lv2���� ��� (i�� max_level�̸� ������� ����)
		if (user.user_exp >= user.user_level[i].required_exp && user.level == user.user_level[i - 1].level) {
			(user.level)++;
			user.max_hp = user.user_level[i].max_hp;
			user.max_tp = user.user_level[i].max_tp;
			user.p_attack = user.user_level[i].p_attack;
			user.p_defense = user.user_level[i].p_defense;
			user.hp = user.max_hp;
			show_status(1, 0);
			gamectl(GAME_SAVE);
			sprintf(buf, "���� ��!\n\nLEVEL %d �� %d", before_level, after_level);
			xprint_init(27, 10, 10, 4);
			xprint(buf, 30, 1);
		}
	}
}

void showEnemyList(void) {
	show_enemy_status_init(1, 2);
	show_enemy_status(0, 0);

	show_enemy_status_init(20, 2);
	show_enemy_status(1, 0);

	show_enemy_status_init(40, 2);
	show_enemy_status(2, 0);
}

int keyInput_selectEnemy(int *cursor, int min_pos, int max_pos) {
	int key = 0;
	setColor(15, 0);

	//���� Ŀ�� ���� �̹� óġ�� ���� ��� Ŀ�� �� �̵� (+1)
	if (*cursor == STR1 && _enemy[0]->active == -1) {
		(*cursor) = STR2;
	}
	if (*cursor == STR2 && _enemy[1]->active == -1) {
		(*cursor) = STR3;
	}
	if (*cursor == STR3 && _enemy[2]->active == -1) {
		(*cursor) = STR1;
	}

	if (_kbhit()) {
		key = _getch();

		//UP DOWN LEFT RIGHT
		if (key == EXTEND_KEY) {
			key = _getch();

			switch (key) {
			case KEY_LEFT:
			case KEY_UP:
				
				if (*cursor > min_pos) (*cursor)--; //cursor ���� min_pos���� ũ�� cursor �� ����
				else (*cursor) = max_pos; //�ƴϸ� cursor ���� max_pos�� ����

				if (*cursor == STR1 && _enemy[0]->active == -1) {
					(*cursor) = STR3;
				}
				else if (*cursor == STR2 && _enemy[1]->active == -1) {
					(*cursor) = STR1;
				}
				else if (*cursor == STR3 && _enemy[2]->active == -1) {
					(*cursor) = STR2;
				}
				break;
			case KEY_RIGHT:
			case KEY_DOWN:
				if (*cursor < max_pos) (*cursor)++; //cursor ���� max_pos���� ������ cursor �� ����
				else (*cursor) = min_pos; //�ƴϸ� cursor ���� min_pos�� ����

				if (*cursor == STR1 && _enemy[0]->active == -1) {
					(*cursor) = STR2;
				}
				else if (*cursor == STR2 && _enemy[1]->active == -1) {
					(*cursor) = STR3;
				}
				else if (*cursor == STR3 && _enemy[2]->active == -1) {
					(*cursor) = STR1;
				}
				break;
			}
		}

		else if (key == KEY_Z) { //'z' Ű�� ������ 0 ��ȯ (show_select_menu()�� �ݺ��� ����)
			return 0;
		}
		else if (key == KEY_X) { //���� ȭ�� ����. XŰ�� ���Ű�� ���
			return -1;
		}
		else if (key == KEY_ESC) {
			if (nScreenIndex != 1)
				menu_pause();
			else {
				(*cursor) = _ESC;
				return 0;
			}
		}
	}

	return 1;
}

void select_enemy(int *cursor, int enemyCount) {
	int flag = 1;
	ScreenPrint(2, 20, "����� ������ �ּ���.");
	while (flag = keyInput_selectEnemy(cursor, 0, enemyCount - 1)) {
		if (flag == -1) { //XŰ�� ���� ���
			*cursor = STR10; //dummy value (�ƹ� �ൿ�� ���� �ʵ��� = ��� ȿ��)
			break;
		}

		//���õ� ���� �÷���, ���õ��� ���� ���� ������� ǥ��
		switch (*cursor) {
		case STR1:
			show_enemy_status_init(1, 2);
			show_enemy_status(0, 0);

			show_enemy_status_init(20, 2);
			show_enemy_status(1, 1);

			show_enemy_status_init(40, 2);
			show_enemy_status(2, 1);
			break;
		case STR2:
			show_enemy_status_init(1, 2);
			show_enemy_status(0, 1);

			show_enemy_status_init(20, 2);
			show_enemy_status(1, 0);

			show_enemy_status_init(40, 2);
			show_enemy_status(2, 1);
			break;
		case STR3:
			show_enemy_status_init(1, 2);
			show_enemy_status(0, 1);

			show_enemy_status_init(20, 2);
			show_enemy_status(1, 1);

			show_enemy_status_init(40, 2);
			show_enemy_status(2, 0);
			break;
		}
		Sleep(ref_speed);
	}
}

int battle_screen(int enemyCount) {
	int cursor = STR1;
	static int cursor2 = STR1;
	int randNum;

	//default x, y position
	int x = WIDTH_SIZE - 12, y = HEIGHT_SIZE - 4;
	char str[][MAX_ARRAY_SIZE] = {
		"> ����",
		"  ��ų",
		"  ����ģ��"
	};
	int row = sizeof(str) / sizeof(str[0]); //�迭�� ���� ũ��

	system("cls");
	if (alreadyEscapeClicked) { //����ģ�� ��ư�� 1ȸ �̻� ���� ���
		str[2][1] = '\0'; //��ư �����
	}
	else {
		str[2][1] = ' '; //�ƴϸ� ���̱�
	}
	show_status_init(1, HEIGHT_SIZE - 2, 26, HEIGHT_SIZE - 2);
	show_status(1, 0);

	showEnemyList();

	show_current_map();
	show_select_menu(x, y, &cursor, str, 10, row);

	switch (cursor) {
	case STR1:
		select_enemy(&cursor2, enemyCount);
		switch (cursor2) {
		case STR1:
			if (_enemy[0]->active != -1) { //���� ��Ȱ��ȭ ���°� �ƴϸ�
				_enemy[0]->active = 1; //���õ� ������ �� ��
				return AttackProcess(AP_PtE); //���� ���μ��� ȣ��
			}	
			break;
		case STR2:
			if (_enemy[1]->active != -1) {
				_enemy[1]->active = 1;
				return AttackProcess(AP_PtE);
			}
			break;
		case STR3:
			if (_enemy[2]->active != -1) {
				_enemy[2]->active = 1;
				return AttackProcess(AP_PtE);
			}
			break;
		default:
			break;
		}
		break;
	case STR2:
		while (usePlayerSkill()); //�̱���
		break;
	case STR3:
		if (!alreadyEscapeClicked) { //����ġ�⸦ 1ȸ�� �õ��� �� �ֵ���
			alreadyEscapeClicked = 1;
			srand((unsigned)time(NULL) + rand());
			randNum = rand() % 1000 + 1;

			if (randNum >= 1 && randNum <= 600) { //60% Ȯ���� ����ġ�� ����
				xprint_init(2, 20, WIDTH_SIZE - 2, 4);
				xprint("������ �����ƴ�...", 30, 1);
				move_anim();
				return 0;
			}
			else {
				xprint_init(2, 20, WIDTH_SIZE - 2, 4);
				xprint("����ĥ �� ����!", 30, 1);
				return 1;
			}
		}
		break;
	default:
		break;
	}

	if (cursor2 >= STR4) //���� dummy�� ���, 1������ ��ȯ
		cursor2 = STR1;

	return 1;
}