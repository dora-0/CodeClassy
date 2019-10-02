#include <stdio.h>
#include "main.h"
#include "db_ctl.h"
/*
	** character.c : 캐릭터 관련 제어

	1) int get_max_level(void)
	** 최대 레벨을 서버로부터 가져오는 함수
	서버로부터 레벨 별 필요 경험치, 해당 레벨 최대 HP, 최대 TP,
	물리 공격력, 물리 방어력 데이터를 가져와 user 구조체 내의 USER_LEVEL 타입 user_level 배열에 저장.

	이후 가져온 행의 갯수를 최대 레벨로 가정하고 최대 레벨 값을 반환.

	===

	2) void show_status_init(int hp_pos_X, int hp_pos_Y, int tp_pos_X, int tp_pos_Y)
	3) void show_status(int show_lvl_name, int is_grayscale_mode)
	** 상태창 구현 함수

	플레이어 레벨, 플레이어 이름, HP 게이지, TP 게이지 등을 출력한다.
	함수 사용 전 show_status_init()으로 먼저 필요한 값들을 미리 설정해야 한다.

	-- show_status_init()
	hp_pos_X : HP 게이지의 X 좌표
	hp_pos_Y : HP 게이지의 Y 좌표
	tp_pos_X : TP 게이지의 X 좌표
	tp_pos_Y : TP 게이지의 Y 좌표

	-- show_status()
	show_lvl_name : 플레이어 레벨 및 이름 표시 여부 (표시 설정 시, HP 게이지 바로 윗쪽에 표시됨)
	is_grayscale_mode : 그레이스케일 모드 여부 (애니메이션 연출 전용)

	===

	4) void show_character_info(void)
	** 캐릭터 정보 표시

	구조체 _CurrentUser에서 정의된 구조체 변수 user로부터 스탯 값을 불러와 출력 (->MySQL DB)

*/

//정적 전역 변수 - show_status_init()을 통해 외부에서 설정 가능
static int stbar_hp_pos_X;
static int stbar_tp_pos_X;
static int stbar_hp_pos_Y;
static int stbar_tp_pos_Y;

//battle.c에서 참조할 값
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

	max_level = i; //가져온 행의 갯수 = 최대 레벨
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
	//HP, TP 게이지 기본 색상
	int hp_bg_color = CC_GREEN;
	int tp_bg_color = CC_CYAN;

	//그레이스케일 모드일 경우 어두운 회색으로 설정
	if (is_grayscale_mode) {
		hp_bg_color = CC_DARKGRAY;
		tp_bg_color = CC_DARKGRAY;
	}

	//HP 비율 계산 (HP, TP 게이지 변화를 위해 사용)
	//최대 HP와 동일할 경우 100%
	if (user.hp == user.max_hp) {
		hp_ratio = 10;
	}
	else if (user.hp == 0) {
		hp_ratio = -1;
	}
	else {
		//hp_ratio 값을 1씩 줄이면서, 플레이어 최대 HP의 10% * hp_ratio보다 현재 HP가 더 큰 경우 for문 종료
		for (hp_ratio = 9; hp_ratio > 0; hp_ratio--) {
			if (user.hp >= user.max_hp * (double)hp_ratio * 0.1) {
				break;
			}
		}
	}

	//TP 비율 계산
	//최대 HP와 동일할 경우 100%
	if (user.tp == user.max_tp) {
		tp_ratio = 10;
	}
	else if (user.tp == 0) {
		tp_ratio = -1;
	}
	else {
		//tp_ratio 값을 1씩 줄이면서, 플레이어 최대 TP의 10% * tp_ratio보다 현재 TP가 더 큰 경우 for문 종료
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
	//BG 색상 변경 후 hp_ratio 만큼 printf로 띄어쓰기
	setBgColor(hp_bg_color);
	for (j = 0; j <= hp_ratio; j++) {
		printf(" ");
	}

	//레이아웃은 일정해야 하므로 BG 색상을 디폴트 값으로 변경 후 10 - hp_ratio 만큼 printf로 띄어쓰기
	resetBgColor();
	for (j = 0; j <= 10 - hp_ratio; j++) {
		printf(" ");
	}

	printf("%3d/%d", user.hp, user.max_hp);

	//tp
	gotoxy(stbar_tp_pos_X, stbar_tp_pos_Y);
	printf("TP ");
	//BG 색상 변경 후 hp_ratio 만큼 printf로 띄어쓰기
	setBgColor(tp_bg_color);
	for (j = 0; j <= tp_ratio; j++) {
		printf(" ");
	}

	//레이아웃은 일정해야 하므로 BG 색상을 디폴트 값으로 변경 후 10 - tp_ratio 만큼 printf로 띄어쓰기
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
	printf("======캐릭터 정보======\n");
	gotoxy(x, y + 1);
	printf("물리 공격력: %10d", user.p_attack);
	gotoxy(x, y + 2);
	printf("물리 방어력: %10d", user.p_defense);
	gotoxy(x, y + 3);
	printf("마법 공격력: %10d", user.m_attack);
	gotoxy(x, y + 4);
	printf("마법 방어력: %10d", user.m_defense);
	gotoxy(x, y + 5);
	if (user.level == max_level)
		printf("     경험치: %4d/%5s", user.user_exp, "-");
	else
		printf("     경험치: %4d/%5d", user.user_exp, user.user_level[user.level].required_exp);
}