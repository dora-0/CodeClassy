#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "sound.h"

/*
	** main.c : 프로그램 시작점

	1) int main(void)
	** 메인 함수

	시스템 초기화, 메뉴 호출 및 BGM 재생 등
	핵심 기능의 로드 및 각종 변수의 초기화를 수행한다.

*/

int main(void) {
	//시스템 초기화
	init();
	fmod_init();

	//메뉴 호출
	play_BGM("BGM\\menu.ogg", FMOD_LOOP_NORMAL);
	while (menu());
	
	ScreenRelease();
	return 0;
}