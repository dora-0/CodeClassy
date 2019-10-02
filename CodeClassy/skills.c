#include <stdio.h>
#include "main.h"

int usePlayerSkill(void) {
	int cursor = STR1;
	//default x, y position
	int x = 2, y = 20;
	char str[][MAX_ARRAY_SIZE] = {
		"> TP 50  :: �ް� ������ - �� 1���� ���ݷ� * 2.0���� ������",
		"  TP 120 :: Ʈ���� ������ - �� 1���� 3���� ������",
		"  TP 200 :: ���÷��� �ͽ��÷��� - �� ��ü���� ���ݷ� * 5.0���� ������"
	};
	int row = sizeof(str) / sizeof(str[0]); //�迭�� ���� ũ��

	show_select_menu(x, y, &cursor, str, 20, row);

	switch (cursor) {
	case STR1:
	case STR2:
	case STR3:
	default:
		drawMenuBorder(24, 8);
		xprint_init(27, 10, 10, 5);
		xprint("�̿ϼ��� ����̹Ƿ�\n����� �� �����ϴ�.", 30, 1);
		xprint_init(24, 8, 35, 9);
		xprint("", 0, 0);
		return 0;
		break;
	}
	return 1;
}