#include <stdio.h>
#include "main.h"

int usePlayerSkill(void) {
	int cursor = STR1;
	//default x, y position
	int x = 2, y = 20;
	char str[][MAX_ARRAY_SIZE] = {
		"> TP 50  :: 메가 슬래시 - 적 1명에게 공격력 * 2.0배의 데미지",
		"  TP 120 :: 트리플 게이저 - 적 1명에게 3연속 데미지",
		"  TP 200 :: 스플래시 익스플로전 - 적 전체에게 공격력 * 5.0배의 데미지"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기

	show_select_menu(x, y, &cursor, str, 20, row);

	switch (cursor) {
	case STR1:
	case STR2:
	case STR3:
	default:
		drawMenuBorder(24, 8);
		xprint_init(27, 10, 10, 5);
		xprint("미완성된 기능이므로\n사용할 수 없습니다.", 30, 1);
		xprint_init(24, 8, 35, 9);
		xprint("", 0, 0);
		return 0;
		break;
	}
	return 1;
}