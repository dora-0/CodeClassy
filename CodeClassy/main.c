#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "sound.h"

/*
	** main.c : ���α׷� ������

	1) int main(void)
	** ���� �Լ�

	�ý��� �ʱ�ȭ, �޴� ȣ�� �� BGM ��� ��
	�ٽ� ����� �ε� �� ���� ������ �ʱ�ȭ�� �����Ѵ�.

*/

int main(void) {
	//�ý��� �ʱ�ȭ
	init();
	fmod_init();

	//�޴� ȣ��
	play_BGM("BGM\\menu.ogg", FMOD_LOOP_NORMAL);
	while (menu());
	
	ScreenRelease();
	return 0;
}