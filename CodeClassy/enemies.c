#include <stdio.h>
#include <time.h>
#include "db_ctl.h"
#include "main.h"
/*
	** enemies.c : �� �̺�Ʈ ó��

	1) struct _EnemyInfo *enemy[MAX_ENEMIES];
	2) struct _EnemyInfo *temp[MAX_ENEMIES];
	�� ������ ������ �����ϴ� ����ü ����.
	enemy : ��ü �� ����
	temp : �ӽ� �� ���� (����� ������ ����� ���� �ҷ��� �� �ӽ÷� ���)

	===

	3) CurrentEnemies *_enemy[3];
	���� ���� �� �� ������ (�ִ� 3��)�� �����ϴ� ����ü ����.

	===

	4) static int enemy_stbar_hp_pos_X;
	5) static int enemy_stbar_hp_pos_Y;
	6) static int _EnemyInfoRows;

	4, 5: show_enemy_status() �Լ��� ���Ǵ� ��
	6: enemies ���̺��� �ҷ��� ���� ������ ������ �� (=�� �������� �ִ� ����)


	===

	7) void show_enemy_status_init(int hp_pos_X, int hp_pos_Y)
	8) void show_enemy_status(int enemy_id, int is_grayscale_mode)

	���� ���� �� ���� ���� �����͸� ����� �� ����Ѵ�.
	show_status() �Լ��� �� ������ ǥ�ÿ����� ����.
	TP ����, enemy_id ���� �߰�
	������ �� �����Ϳ� boss_flag�� �����Ǿ� ������ ���� �������� ǥ��

	===

	9) int getEnemyInfo(void)
	�����κ��� �� �����͸� �������� �Լ�.
	�� �����͸� �ٷ�� �� ���� ���� ȣ��Ǿ�� ��.

	===

	10) int SmartFetchEnemies(int enemy_id)
	11) int setCurrentEnemies(int enemy1_id, int enemy2_id, int enemy3_id)
	���� ���� �� ���� �� �����͸� ����/����.

	SmartFetchEnemies()�� ����� �����κ��� �÷��̾� ������ ����� �� �����͸� �ҷ���.
	���� ������ ���� ������ �� �ϳ��� �������� ��� 1~3�� ���� ������.

	===

	12) int freeEnemyInfo(void)
	13) int freeCurrentEnemies(void)

	�޸� ���� �Լ�.

*/

struct _EnemyInfo *enemy[MAX_ENEMIES];
struct _EnemyInfo *temp[MAX_ENEMIES];
CurrentEnemies *_enemy[3];
static int enemy_stbar_hp_pos_X;
static int enemy_stbar_hp_pos_Y;

static int _EnemyInfoRows;

void show_enemy_status_init(int hp_pos_X, int hp_pos_Y)
{
	enemy_stbar_hp_pos_X = hp_pos_X;
	enemy_stbar_hp_pos_Y = hp_pos_Y;
}

void show_enemy_status(int enemy_id, int is_grayscale_mode) {
	int j = 0;
	int hp_ratio = 0;
	char hp_string[MAX_ARRAY_SIZE];
	//HP ������ �⺻ ����
	int hp_bg_color = CC_GREEN;

	if (is_grayscale_mode) {
		hp_bg_color = CC_DARKGRAY;
	}

	if (_enemy[enemy_id]->enemy_id == -1)
		return;

	if (_enemy[enemy_id]->boss_flag) {
		//BOSS HP ���� ��� (HP ������ ��ȭ�� ���� ���)
		//�ִ� HP�� ������ ��� 100%
		if (_enemy[enemy_id]->hp == _enemy[enemy_id]->max_hp) {
			hp_ratio = 50;
		}
		else if (_enemy[enemy_id]->hp == 0) { //hp�� 0�̸� 0%
			hp_ratio = -1;
		}
		else {
			//hp_ratio ���� 1�� ���̸鼭, �ִ� HP�� 10% * hp_ratio���� ���� HP�� �� ū ��� for�� ����
			for (hp_ratio = 49; hp_ratio > 0; hp_ratio--) {
				if (_enemy[enemy_id]->hp >= _enemy[enemy_id]->max_hp * (double)hp_ratio * 0.02) {
					break;
				}
			}
		}
	}
	else {
		//HP ���� ��� (HP, TP ������ ��ȭ�� ���� ���)
		//�ִ� HP�� ������ ��� 100%
		if (_enemy[enemy_id]->hp == _enemy[enemy_id]->max_hp) {
			hp_ratio = 10;
		}
		else if (_enemy[enemy_id]->hp == 0) { //hp�� 0�̸� 0%
			hp_ratio = -1;
		}
		else {
			//hp_ratio ���� 1�� ���̸鼭, �ִ� HP�� 10% * hp_ratio���� ���� HP�� �� ū ��� for�� ����
			for (hp_ratio = 9; hp_ratio > 0; hp_ratio--) {
				if (_enemy[enemy_id]->hp >= _enemy[enemy_id]->max_hp * (double)hp_ratio * 0.1) {
					break;
				}
			}
		}

	}

	gotoxy(enemy_stbar_hp_pos_X, enemy_stbar_hp_pos_Y - 1);
	printf("Lv. %d %s", _enemy[enemy_id]->level, _enemy[enemy_id]->name);

	if (_enemy[enemy_id]->boss_flag) { //boss_flag�� �����Ǿ� ������
		gotoxy(enemy_stbar_hp_pos_X + 20, enemy_stbar_hp_pos_Y - 1);
		sprintf(hp_string, "%d/%d", _enemy[enemy_id]->hp, _enemy[enemy_id]->max_hp);
		printf("%34s", hp_string);
	}
	else {
		gotoxy(enemy_stbar_hp_pos_X, enemy_stbar_hp_pos_Y + 1);
		sprintf(hp_string, "%d/%d", _enemy[enemy_id]->hp, _enemy[enemy_id]->max_hp);
		printf("%14s", hp_string);
	}
	
	//hp
	gotoxy(enemy_stbar_hp_pos_X, enemy_stbar_hp_pos_Y);
	printf("HP ");
	//BG ���� ���� �� hp_ratio ��ŭ printf�� ����
	setBgColor(hp_bg_color);
	for (j = 0; j <= hp_ratio; j++) {
		printf(" ");
	}

	resetBgColor();

	if (_enemy[enemy_id]->boss_flag) {
		for (j = 0; j <= 50 - hp_ratio; j++) {
			printf(" ");
		}
	}
	else {
		for (j = 0; j <= 10 - hp_ratio; j++) {
			printf(" ");
		}
	}
	
	//_getch();
}

int getEnemyInfo(void) { //�����κ��� �� ������ ��������
	int i = 0;
	DB_CONNECT();
	SQL_QUERY("select * from enemies");

	while ((db.sql_row = mysql_fetch_row(db.sql_result)) != NULL) {
		enemy[i] = (struct _EnemyInfo *)malloc(sizeof(struct _EnemyInfo));
		enemy[i]->enemy_id = atoi(db.sql_row[0]);
		sprintf(enemy[i]->name, "%s", db.sql_row[1]);
		enemy[i]->level = atoi(db.sql_row[2]);
		enemy[i]->max_hp = atoi(db.sql_row[3]);
		enemy[i]->p_attack = atoi(db.sql_row[4]);
		enemy[i]->p_defense = atoi(db.sql_row[5]);
		enemy[i]->m_attack = atoi(db.sql_row[6]);
		enemy[i]->m_defense = atoi(db.sql_row[7]);
		enemy[i]->boss_flag = atoi(db.sql_row[8]);
		enemy[i]->hp = enemy[i]->max_hp;
		i++;
	}

	_EnemyInfoRows = i;

	/*for (i = 0; i < _EnemyInfoRows; i++) {
		printf("====================\n");
		printf("Enemy ID: %d\n", enemy[i]->enemy_id);
		printf("�̸�: %s\n", enemy[i]->name);
		printf("����: %d\n", enemy[i]->level);
		printf("�ִ� HP: %d\n", enemy[i]->max_hp);
		printf("���� ���ݷ�: %d\n", enemy[i]->p_attack);
		printf("���� ����: %d\n", enemy[i]->p_defense);
		printf("���� ���ݷ�: %d\n", enemy[i]->m_attack);
		printf("���� ����: %d\n", enemy[i]->m_defense);
		printf("BOSS: %d\n", enemy[i]->boss_flag);
	}*/

	db_disconnect();
	//_getch();
	return 0;
}

int SmartFetchEnemies(int enemy_id) { //�÷��̾�� ����� ������ �� ������ ��������
	int i = 0, num_row = 0;
	char buf[512];

	DB_CONNECT();
	sprintf(buf, "select * from enemies where enemy_id = '%d' AND level <= '%d' AND level >= '%d'", enemy_id, user.level, (user.level) - 2);
	SQL_QUERY(buf);

	while ((db.sql_row = mysql_fetch_row(db.sql_result)) != NULL) {
		temp[i] = (struct _EnemyInfo *)malloc(sizeof(struct _EnemyInfo));
		temp[i]->enemy_id = atoi(db.sql_row[0]);
		//temp[i]->name = db.sql_row[1];
		sprintf(temp[i]->name, "%s", db.sql_row[1]);
		temp[i]->level = atoi(db.sql_row[2]);
		temp[i]->max_hp = atoi(db.sql_row[3]);
		temp[i]->p_attack = atoi(db.sql_row[4]);
		temp[i]->p_defense = atoi(db.sql_row[5]);
		temp[i]->m_attack = atoi(db.sql_row[6]);
		temp[i]->m_defense = atoi(db.sql_row[7]);
		temp[i]->boss_flag = atoi(db.sql_row[8]);
		temp[i]->hp = enemy[i]->max_hp;
		i++;
	}

	num_row = i;
	db_disconnect();

	return num_row;
}

int setCurrentEnemies(int enemy1_id, int enemy2_id, int enemy3_id) { //���� �� ����
	int i = 0, randNum = 0, num_row = 0;

	for (i = 0; i < 3; i++) {
		_enemy[i] = (CurrentEnemies *)malloc(sizeof(CurrentEnemies));
	}


	if (enemy1_id != -1) { //enemyX_id�� -1�� �ƴ� ��
		num_row = SmartFetchEnemies(enemy1_id); //num_row : ������ ���� ����

		srand((unsigned)time(NULL) + rand());
		if (num_row == 1)
			randNum = 0;
		else
			randNum = rand() % num_row;

		_enemy[0]->enemy_id = temp[randNum]->enemy_id;
		//_enemy[0]->name = temp[randNum]->name;
		sprintf(_enemy[0]->name, "%s", temp[randNum]->name);
		_enemy[0]->level = temp[randNum]->level;
		_enemy[0]->hp = temp[randNum]->max_hp;
		_enemy[0]->max_hp = temp[randNum]->max_hp;
		_enemy[0]->p_attack = temp[randNum]->p_attack;
		_enemy[0]->p_defense = temp[randNum]->p_defense;
		_enemy[0]->m_attack = temp[randNum]->m_attack;
		_enemy[0]->m_defense = temp[randNum]->m_defense;
		_enemy[0]->boss_flag = temp[randNum]->boss_flag;
		_enemy[0]->active = 0; //-1 : disabled, 0: alive, 1: user selected

		//free
		for (i = 0; i < num_row; i++) {
			free(temp[i]);
		}
	}
	else {
		_enemy[0]->enemy_id = -1;
		_enemy[0]->active = -1; //��Ȱ��ȭ
	}

	if (enemy2_id != -1) {
		num_row = SmartFetchEnemies(enemy2_id);
		srand((unsigned)time(NULL) + rand());
		if (num_row == 1)
			randNum = 0;
		else
			randNum = rand() % num_row;

		_enemy[1]->enemy_id = temp[randNum]->enemy_id;
		//_enemy[1]->name = temp[randNum]->name;
		sprintf(_enemy[1]->name, "%s", temp[randNum]->name);
		_enemy[1]->level = temp[randNum]->level;
		_enemy[1]->hp = temp[randNum]->max_hp;
		_enemy[1]->max_hp = temp[randNum]->max_hp;
		_enemy[1]->p_attack = temp[randNum]->p_attack;
		_enemy[1]->p_defense = temp[randNum]->p_defense;
		_enemy[1]->m_attack = temp[randNum]->m_attack;
		_enemy[1]->m_defense = temp[randNum]->m_defense;
		_enemy[1]->boss_flag = temp[randNum]->boss_flag;
		_enemy[1]->active = 0;

		//free
		for (i = 0; i < num_row; i++) {
			free(temp[i]);
		}
	}
	else {
		_enemy[1]->enemy_id = -1;
		_enemy[1]->active = -1;
	}
	
	if (enemy3_id != -1) {
		num_row = SmartFetchEnemies(enemy3_id);
		srand((unsigned)time(NULL) + rand());
		if (num_row == 1)
			randNum = 0;
		else
			randNum = rand() % num_row;

		_enemy[2]->enemy_id = temp[randNum]->enemy_id;
		//_enemy[2]->name = temp[randNum]->name;
		sprintf(_enemy[2]->name, "%s", temp[randNum]->name);
		_enemy[2]->level = temp[randNum]->level;
		_enemy[2]->hp = temp[randNum]->max_hp;
		_enemy[2]->max_hp = temp[randNum]->max_hp;
		_enemy[2]->p_attack = temp[randNum]->p_attack;
		_enemy[2]->p_defense = temp[randNum]->p_defense;
		_enemy[2]->m_attack = temp[randNum]->m_attack;
		_enemy[2]->m_defense = temp[randNum]->m_defense;
		_enemy[2]->boss_flag = temp[randNum]->boss_flag;
		_enemy[2]->active = 0;

		//free
		for (i = 0; i < num_row; i++) {
			free(temp[i]);
		}
	}
	else {
		_enemy[2]->enemy_id = -1;
		_enemy[2]->active = -1;
	}
	return 0;
}

int freeEnemyInfo(void) {
	int i = 0;

	for (i = 0; i < _EnemyInfoRows; i++) {
		free(enemy[i]);
	}

	return 0;
}

int freeCurrentEnemies(void) {
	int i = 0;

	for (i = 0; i < 3; i++) {
		free(_enemy[i]);
	}

	return 0;
}