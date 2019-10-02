#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "sound.h"
static int ref_speed = 60; //while (keyInput(&cursor2, 0, enemyCount - 1))
static int alreadyEscapeClicked = 0;
extern CurrentEnemies *_enemy[3];
/*
	** battle.c : 전투 제어

	1) struct _BattleInfo result
	** 전투 중 데이터를 저장할 구조체

	전투 진행 중 획득한 경험치, 골드를 임시로 담아둔다.
	
	===

	2) void ResetEscapeClickFlag(void)
	** 도망치기 버튼 관련 함수

	전역 정적 변수인 alreadyEscapeClicked는 도망치기를 1회만 시도할 수 있도록 하기 위해
	도망치기 버튼이 입력되면 값이 1로 변경된다.
	단, 새로운 전투가 시작되면 다시 도망치기를 사용할 수 있도록 해야 하므로
	해당 함수를 호출하면 변수 값을 0으로 변경시킨다.

	===

	3) int AttackProcess(int mode)
	** 전투 프로세스

	int mode :
	AP_PtE : Player to Enemy (적을 공격할 때)
	AP_EtP : Enemy to Player (적에게 공격받을 때)

	턴 순서 :
	플레이어 -> 적 1 -> 적 2 -> 적 3 -> 플레이어 -> ... (단 적의 턴은 생존 상태일 때만 발동)

	===

	4) int hit_damage(int *target, int damage)
	** 데미지 피격 처리 함수

	PtE, EtP 공통으로 사용가능.
	타겟으로 지정된 상대가 체력이 0이 되었는지 여부를 반환한다. (return killed)

	===

	5) void tp_up(int damage)
	** TP 상승 처리 함수 (플레이어 전용)

	적에게 피격받으면 일정 수치만큼 플레이어의 TP를 상승시킨다.
	공식은 (받은 데미지) / (플레이어의 최대 HP) * 100

	===

	6) void game_over(void)
	** 게임 오버 시 호출

	플레이어의 체력이 0이 되어 전투불능이 된 경우 호출됨.

	플레이어 HP, TP 초기화 + 이번 전투 때 획득한 EXP, 골드 초기화.

	===

	7) void battle_result(void)
	** 전투 승리 시 호출

	플레이어가 한 전투에서 모든 적을 쓰러뜨리면 호출됨.
	이번 전투 때 획득한 EXP, 골드를 서버 데이터에 반영.

	이후 레벨 업 체크 함수를 호출함.

	===

	8) void lvlup_check(void)
	** 레벨 업 체크 함수

	서버의 user_levels 테이블에서 데이터를 가져와 레벨 업이 되었는지(=필요 EXP 이상이 되었는지) 체크.
	레벨 업이 된 경우 플레이어의 레벨 업 이후 최대 HP, 최대 TP, 공격력, 방어력 수치를 업데이트 하고
	현재 체력을 풀 회복.

	===

	9) void showEnemyList(void)
	** 적 정보 출력 함수 (단순 출력용)

	===

	9) int keyInput_selectEnemy(int *cursor, int min_pos, int max_pos)
	10) void select_enemy(int *cursor, int enemyCount)
	** 공격 버튼 선택 시 적 선택 처리

	선택된 적은 컬러로, 선택되지 않으면 회색으로 표시.
	keyInput_selectEnemy : select_enemy 함수에 맞게 일부 기능을 변경한 keyInput() 함수 (처치한 적 판정)
	입력받은 인자 값인 커서 값을 키 선택값에 따라 조정

	===

	12) int battle_screen(void)
	** 전투 화면 구현

	전투 발생 시 출력할 화면을 구성한다.
	공격 / 스킬 / 도망친다 (=종료)

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
		if (_enemy[i]->active == 1) { //select_enemy() -> battle_screen()에서 선택된 적 찾기
			active_enemy = i;
			break;
		}
	}

	if (mode == AP_PtE) { //플레이어가 적을 공격할 때
		damage = user.p_attack - _enemy[active_enemy]->p_defense; //최종 데미지: 플레이어의 물리 공격력 - 선택된 적의 물리 방어력
		if (damage < 1) damage = 0;

		enemy_killed = hit_damage(&(_enemy[active_enemy]->hp), damage); //피격 처리 함수 호출 (적 처치 여부 반환)

		showEnemyList();
		play_SE("SE\\att-sound1-9.ogg", FMOD_DEFAULT);
		sprintf(buf, "%s의 공격으로 %s에게 %d의 데미지를 입혔다!", user.username, _enemy[active_enemy]->name, damage);
		xprint_init(2, 20, WIDTH_SIZE - 2, 4);
		xprint(buf, 30, 1);

		if (enemy_killed) { //적이 처치된 경우
			_enemy[active_enemy]->active = -1; //적 비활성화
			result.get_exp += 10 * (_enemy[active_enemy]->level); //획득 경험치: 선택된 적의 레벨 * 10
			result.gold += (_enemy[active_enemy]->p_attack * _enemy[active_enemy]->level); //획득 골드: 선택된 적의 물리 공격력 * 레벨
			sprintf(buf, "%s(은)는 쓰러졌다!", _enemy[active_enemy]->name);
			xprint_init(2, 21, 0, 0);
			xprint(buf, 30, 1);
			battle_result_flag = 1; //flag = 1로 우선 설정

			for (i = 0; i < 3; i++) {
				if (_enemy[i]->active != -1) //아직 적이 남아 있으면 flag = 0
					battle_result_flag = 0;
			}

			if (battle_result_flag) { //적이 모두 처치된 경우
				battle_result(); //호출
				return 0;
			}
		}
	}

	else if (mode == AP_EtP) { //플레이어가 적에게 공격받을 때
		int i;
		for (i = 0; i < 3; i++) { //적 1~3번까지 순서대로 공격
			if (_enemy[i]->active != -1) { //비활성화된 적은 제외
				xprint_init(2, 20, WIDTH_SIZE - 2, 4);
				damage = _enemy[i]->p_attack - user.p_defense; //최종 데미지: 적의 물리 공격력 - 플레이어의 물리 방어력
				if (damage < 1) damage = 0;

				user_killed = hit_damage(&(user.hp), damage); //피격 처리 함수 호출 (플레이어 전투불능 여부 반환)
				tp_up(damage); //TP 상승 처리 함수 호출

				show_status(1, 0);
				sprintf(buf, "ENEMY %d: 적의 공격! %s에게 %d만큼의 데미지를 입었다!", i + 1, _enemy[i]->name, damage);

				xprint(buf, 30, 1);
			}

			if (user_killed) { //플레이어가 전투불능이 된 경우
				//battle_result_flag = 1;
				game_over(); //게임오버 함수 호출
				return 0;
			}
		}
	}

	//선택된 적 해제
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

int hit_damage(int *target, int damage) { //피격 처리 (타겟 = 타겟의 현재 HP 변수, 데미지)
	int killed = 0;

	if ((*target - damage) <= 0) { //현재 HP - 데미지가 0 이하이면 사망 판정
		*target = 0;
		killed = 1;
	}
	else {
		*target -= damage;
	}
	
	return killed; //사망 여부 반환
}

void tp_up(int damage) { //TP 상승 처리
	user.tp += (int)(((double)damage / user.max_hp) * 100);
	if (user.tp > user.max_tp) user.tp = user.max_tp;
}

void game_over(void) { //게임오버 화면
	xprint_init(2, 20, WIDTH_SIZE - 2, 4);
	setFontColor(CC_RED);
	xprint("전투불능 상태가 되었습니다.", 30, 1);
	setFontColor(CC_WHITE);
	user.hp = user.max_hp;
	user.tp = 0;
	result.get_exp = 0;
	result.gold = 0;
}

void battle_result(void) { //전투승리 화면
	char buf[MAX_ARRAY_SIZE];

	user.user_exp += result.get_exp;
	user.gold += result.gold;
	drawMenuBorder(24, 8);
	sprintf(buf, "전투 종료!\n\nEXP +%d\nGOLD +%d", result.get_exp, result.gold);
	result.get_exp = 0;
	result.gold = 0;
	xprint_init(27, 10, 0, 0);
	xprint(buf, 30, 1);
	lvlup_check();
}

void lvlup_check(void) { //레벨 업 체크
	int i;
	int before_level = user.level;
	int after_level = user.level + 1;
	int lvlup_flag = 1;
	char buf[MAX_ARRAY_SIZE];

	int max_level = get_max_level();

	for (i = user.level; i < max_level; i++) { //Level 1인 경우, user.user.level[1] -> 즉 Lv2부터 계산 (i가 max_level이면 계산하지 않음)
		if (user.user_exp >= user.user_level[i].required_exp && user.level == user.user_level[i - 1].level) {
			(user.level)++;
			user.max_hp = user.user_level[i].max_hp;
			user.max_tp = user.user_level[i].max_tp;
			user.p_attack = user.user_level[i].p_attack;
			user.p_defense = user.user_level[i].p_defense;
			user.hp = user.max_hp;
			show_status(1, 0);
			gamectl(GAME_SAVE);
			sprintf(buf, "레벨 업!\n\nLEVEL %d → %d", before_level, after_level);
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

	//현재 커서 값이 이미 처치한 적인 경우 커서 값 이동 (+1)
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
				
				if (*cursor > min_pos) (*cursor)--; //cursor 값이 min_pos보다 크면 cursor 값 감소
				else (*cursor) = max_pos; //아니면 cursor 값을 max_pos로 변경

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
				if (*cursor < max_pos) (*cursor)++; //cursor 값이 max_pos보다 작으면 cursor 값 증가
				else (*cursor) = min_pos; //아니면 cursor 값을 min_pos로 변경

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

		else if (key == KEY_Z) { //'z' 키를 누르면 0 반환 (show_select_menu()의 반복문 종료)
			return 0;
		}
		else if (key == KEY_X) { //전투 화면 전용. X키를 취소키로 사용
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
	ScreenPrint(2, 20, "대상을 선택해 주세요.");
	while (flag = keyInput_selectEnemy(cursor, 0, enemyCount - 1)) {
		if (flag == -1) { //X키를 누른 경우
			*cursor = STR10; //dummy value (아무 행동도 하지 않도록 = 취소 효과)
			break;
		}

		//선택된 적은 컬러로, 선택되지 않은 적은 흑백으로 표시
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
		"> 공격",
		"  스킬",
		"  도망친다"
	};
	int row = sizeof(str) / sizeof(str[0]); //배열의 세로 크기

	system("cls");
	if (alreadyEscapeClicked) { //도망친다 버튼을 1회 이상 누른 경우
		str[2][1] = '\0'; //버튼 숨기기
	}
	else {
		str[2][1] = ' '; //아니면 보이기
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
			if (_enemy[0]->active != -1) { //적이 비활성화 상태가 아니면
				_enemy[0]->active = 1; //선택된 적으로 한 후
				return AttackProcess(AP_PtE); //전투 프로세스 호출
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
		while (usePlayerSkill()); //미구현
		break;
	case STR3:
		if (!alreadyEscapeClicked) { //도망치기를 1회만 시도할 수 있도록
			alreadyEscapeClicked = 1;
			srand((unsigned)time(NULL) + rand());
			randNum = rand() % 1000 + 1;

			if (randNum >= 1 && randNum <= 600) { //60% 확률로 도망치기 가능
				xprint_init(2, 20, WIDTH_SIZE - 2, 4);
				xprint("무사히 도망쳤다...", 30, 1);
				move_anim();
				return 0;
			}
			else {
				xprint_init(2, 20, WIDTH_SIZE - 2, 4);
				xprint("도망칠 수 없다!", 30, 1);
				return 1;
			}
		}
		break;
	default:
		break;
	}

	if (cursor2 >= STR4) //값이 dummy일 경우, 1번으로 전환
		cursor2 = STR1;

	return 1;
}