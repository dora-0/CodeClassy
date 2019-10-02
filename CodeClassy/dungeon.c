#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
/*
	** dungeon.c : ���� �̺�Ʈ ó��

	1) static int (*d_event[6])()
	** �Լ� ������ ������ ���� �̺�Ʈ ȣ�� �Լ�.
	���: d_event[enum EventFuncList];

	===
	
	2) int d_EventInit(void)
	** ���� �̺�Ʈ �ʱ�ȭ �Լ�. (���� ���� ȣ���)
	���� �÷��̾� ��ġ�� mask ���� ���� �پ��� �̺�Ʈ�� ����.

	===

	3) int d_randomEvent(void) (�̻��)
	��Ʋ �̺�Ʈ �ߵ� �� ���� Ȯ���� ������ ȹ�� �̺�Ʈ�� �߻���Ű�� ���� �Լ�.
	�ð� �������� ���� X

	===

	4) void d_Message_Blocked(void)
	���� �������� �̵� �õ� �� ��� �޽���

	===

	5) void d_movePos(int mode)
	** ������ map.c - move_position()�� d_movePos()�� ����
	** ������� ��ǥ�� �̵���Ű�� �Լ�
	���ڰ� map�� enum map_position_move_mode�� �����͸� ����Ѵ�.

	����ڰ� ��ġ�� ��ǥ�� mask ���� ��� (Up or Down)�̶�� ��ǥ �̵� �õ� �� �ش� mask ���� ����. (��� ������ �� �ֵ���)
	�ƴ϶�� �̵� �õ� �� mask ���� -1�� ����. (�̺�Ʈ ����)

	����ڰ� ������ �ϴ� ������ �� ���� �� is_blocked ���� 1(���� �Ұ�)�̶��,
	�ش� ��ġ�� �̵��� �� ���� �ϸ� �ƴ϶�� ������� ��ǥ ���� �����ϰ�
	�ش� ���� is_first ���� 0���� �����.

	�̵� ��, move_anim()�� ȣ���Ѵ�.

	+ MOVE_AUTO �ɼ� �߰�
	���� �ٲ� �� ��� ��ġ�� �ٷ� �̵��ϰ� �Ǹ� ���װ� ����Ƿ� MOVE_AUTO �ɼ����� �̵��Ѵ�.
	MOVE_AUTO �ɼ��� ����ڰ� ���� ��ġ�� ��ǥ�� �����¿츦 üũ�� ���� ������ ������ �ڵ����� �̵��Ѵ�.

	��� ���� �� ���� �̻��� ���� �����Ƿ� ��� ���� �� MOVE_AUTO �ɼ����� ������κ��� ��ĭ ������ ������ ��.

	===

	6) int _default(void)
	** ���� �̺�Ʈ : �⺻ (d_event() �Լ� �����ͷ� ȣ��)

	�̵� ��ġ ����

	===

	7) int _battle(void)
	** ���� �̺�Ʈ : ���� (d_event() �Լ� �����ͷ� ȣ��)

	���� �߻� �̺�Ʈ

	===

	8) int _item(void)
	9) int _special(void)

	������ ȹ�� �̺�Ʈ (�̱���)
	Ư�� �̺�Ʈ (�̱���)

	===

	10) int _up(void)
	11) int _down(void)

	��� �ö󰡱�/�������� �̺�Ʈ

	-> �� �̵�

*/

#define _NULL -1
extern struct MAP *map[6];
static int (*d_event[6])() = { _default, _battle, _item, _up, _down, _special }; //�Լ� ������

const enum EventFuncList {
	DEFAULT = 0,
	BATTLE = 1,
	ITEM = 2,
	S_UP = 3,
	S_DOWN = 4,
	SPECIAL = 5
};

const enum EventMaskList {
	NONE = -1,
	BATTLE_ZONE = 0,
	BLOCKED = 1,
	SP_EVENT = 7,
	STAIR_UP = 8,
	STAIR_DOWN = 9
};

//Event List:
//���� �̺�Ʈ
//Ư�� �̺�Ʈ(Ư�� �� ����)
//�̵� ��� �̺�Ʈ

int d_EventInit(void) {
	switch (map[MAPID]->level[USERY][USERX]->mask) {
	case STAIR_UP:
		if (d_event[S_UP]()) //��ȯ���� 1�̸� (�ö󰣴� ����)
			(MAPID)++;
		d_movePos(MOVE_AUTO); //�ڵ����� ���� �������� 1ĭ �̵�
		return 1;
		break;
	case STAIR_DOWN:
		if (d_event[S_DOWN]()) //��ȯ���� 1�̸� (�������� ����)
			(MAPID)--;
		d_movePos(MOVE_AUTO); //�ڵ����� ���� �������� 1ĭ �̵�
		return 1;
		break;
	case SP_EVENT:
		return d_event[SPECIAL]();
		break;
	case BATTLE_ZONE:
		//d_event[d_randomEvent()]();
		if (!d_event[BATTLE]()) {
			return 0;
		}
	case NONE:
		return d_event[DEFAULT]();
		break;
	}

	return 1;
}

int d_randomEvent(void) { //�̻��
	int EventFunc;

	srand((unsigned)time(NULL));
	EventFunc = rand() % 2 + 1;

	return EventFunc;
}

void d_Message_Blocked(void) { //���� �������� �̵� �õ� �� ���
	drawMenuBorder(24, 8);
	xprint_init(27, 10, 10, 5);
	xprint("�� �������δ�\n�̵��� �� ���� �� ����.", 30, 1);
}

void d_movePos(int mode) {
	switch (map[MAPID]->level[USERY][USERX]->mask) {
	case STAIR_UP:
	case STAIR_DOWN:
		break;
	default:
		map[MAPID]->level[USERY][USERX]->mask = NONE;
	}

	switch (mode) {
	case MOVE_UP:
		if (map[MAPID]->level[(USERY)-1][USERX]->mask == BLOCKED) {
			d_Message_Blocked();
		}			
		else {
			map[MAPID]->level[(USERY)-1][USERX]->is_first = 0;
			(USERY)--;
		}
		break;
	case MOVE_DOWN:
		if (map[MAPID]->level[(USERY)+1][USERX]->mask == BLOCKED) {
			d_Message_Blocked();
		}
		else {
			map[MAPID]->level[(USERY)+1][USERX]->is_first = 0;
			(USERY)++;
		}
		break;
	case MOVE_LEFT:
		if (map[MAPID]->level[USERY][(USERX)-1]->mask == BLOCKED) {
			d_Message_Blocked();
		}
		else {
			map[MAPID]->level[USERY][(USERX)-1]->is_first = 0;
			(USERX)--;
		}
		break;
	case MOVE_RIGHT:
		if (map[MAPID]->level[USERY][(USERX)+1]->mask == BLOCKED) {
			d_Message_Blocked();
		}
		else {
			map[MAPID]->level[USERY][(USERX)+1]->is_first = 0;
			(USERX)++;
		}
		break;
	case MOVE_AUTO:
		if (map[MAPID]->level[USERY][(USERX)+1]->mask != BLOCKED) {
			map[MAPID]->level[USERY][(USERX)+1]->is_first = 0;
			(USERX)++;
		}
			
		else if (map[MAPID]->level[USERY][(USERX)-1]->mask != BLOCKED) {
			map[MAPID]->level[USERY][(USERX)-1]->is_first = 0;
			(USERX)--;
		}
			
		else if (map[MAPID]->level[(USERY)+1][USERX]->mask != BLOCKED) {
			map[MAPID]->level[(USERY)+1][USERX]->is_first = 0;
			(USERY)++;
		}
			
		else if (map[MAPID]->level[(USERY)-1][USERX]->mask != BLOCKED) {
			map[MAPID]->level[(USERY)-1][USERX]->is_first = 0;
			(USERY)--;
		}
			
		break;
	default:
		break;
	}

	//���̵� �ƿ�-���̵� �� �ִϸ��̼�
	move_anim();
}


int _default(void) {
	static int cursor = STR1;
	//default x, y position
	int x = 2, y = 8;
	char str[][MAX_ARRAY_SIZE] = {
		"> Up",
		"  Down",
		"  Left",
		"  Right",
		"  ",
		"  Exit"
	};
	int row = sizeof(str) / sizeof(str[0]); //�迭�� ���� ũ��

	system("cls");

	show_status_init(1, HEIGHT_SIZE - 2, 26, HEIGHT_SIZE - 2);
	show_status(1, 0);

	show_current_map();
	LPRINT(x, 1, "���� ����?");
	show_select_menu(x, y, &cursor, str, 10, row);

	switch (cursor) {
	case STR1:
		d_movePos(MOVE_UP);
		break;
	case STR2:
		d_movePos(MOVE_DOWN);
		break;
	case STR3:
		d_movePos(MOVE_LEFT);
		break;
	case STR4:
		d_movePos(MOVE_RIGHT);
		break;
	case STR6:
		return 0;
		break;
	default:
		return d_event[DEFAULT](); //��ĭ�� ������ ��� Default �̺�Ʈ ��ȣ�� (���� ���� ����)
		break;
	}

	return 1;
}

int _battle(void) {
	int enemyCount = 0;
	int enemyLevel = 0;

	srand((unsigned)time(NULL));
	enemyCount = rand() % 3 + 1;

	switch (enemyCount) { //���� �� 1 ~ 3 ���� ����
	case 1:
		setCurrentEnemies(1, _NULL, _NULL); //_NULL == -1, -1�� �Է� �� ��Ȱ��ȭ �� ������ �Ǵ�
		break;
	case 2:
		setCurrentEnemies(1, 1, _NULL);
		break;
	case 3:
		setCurrentEnemies(1, 1, 1);
		break;
	}
	
	ResetEscapeClickFlag();
	while (battle_screen(enemyCount)) {
		if (!AttackProcess(AP_EtP)) { //���� ���μ��� (ETP ���)���� ��ȯ���� 0�̶�� (=���� ����)
			return 0; //_battle() -> d_EventInit()���� 0 ��ȯ (���� ������)
		}
	}
	freeCurrentEnemies();
	return 1;
}

int _item(void) {
	printf("Item Event!!\n"); //������ ȹ�� �̺�Ʈ (�̱���)
	Sleep(500);
	return 1;
}

int _up(void) { //��� �ö󰡴� �̺�Ʈ
	static int cursor = STR2;
	//default x, y position
	int x = 2, y = 8;
	char str[][MAX_ARRAY_SIZE] = {
		"  �ö󰣴�",
		"> ���ư���"
	};
	int row = sizeof(str) / sizeof(str[0]); //�迭�� ���� ũ��

	system("cls");

	show_status_init(1, HEIGHT_SIZE - 2, 26, HEIGHT_SIZE - 2);
	show_status(1, 0);

	show_current_map();
	LPRINT(x, 1, "�ö󰡴� ����� �ִ�.");
	show_select_menu(x, y, &cursor, str, 10, row);

	switch (cursor) {
	case STR1:
		return 1;
		break;
	case STR2:
		break;
	}

	return 0;
}

int _down(void) { //��� �������� �̺�Ʈ
	static int cursor = STR2;
	//default x, y position
	int x = 2, y = 8;
	char str[][MAX_ARRAY_SIZE] = {
		"  ��������",
		"> ���ư���"
	};
	int row = sizeof(str) / sizeof(str[0]); //�迭�� ���� ũ��

	system("cls");

	show_status_init(1, HEIGHT_SIZE - 2, 26, HEIGHT_SIZE - 2);
	show_status(1, 0);

	show_current_map();
	LPRINT(x, 1, "�������� ����� �ִ�.");
	show_select_menu(x, y, &cursor, str, 10, row);

	switch (cursor) {
	case STR1:
		return 1;
		break;
	case STR2:
		break;
	}

	return 0;
}

int _special(void) { //Ư�� �̺�Ʈ (�̱���)
	printf("SP Event!!\n");
	Sleep(500);
	return d_event[DEFAULT]();
}