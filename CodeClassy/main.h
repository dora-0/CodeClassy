#pragma once
#pragma warning (disable:4996)
#define SAME(x, y) strcmp(x, y) == 0
#define NBSP(x) strtok(x, " ")
//game.dll
#include "utils.h"

void init(void);

//error.c
int fail_connect(void);
int fail_query(const char *query);

//game.c
enum gamectl_commands {
	GAME_INIT,
	GAME_START,
	GAME_SAVE
};
int gamectl(int cmd);
int home(void);

//account.c
int FlushArray(char *arr, const int ArraySize);
int InputID(char *userID);
int InputPW(char *password);
int joinUser(void);
int Login(void);

//ui.c
#define MAX_ARRAY_SIZE 512
enum menu_cursor {
	STR1,
	STR2,
	STR3,
	STR4,
	STR5,
	STR6,
	STR7,
	STR8,
	STR9,
	STR10,
	_ESC = -1
};

enum keymap {
	KEY_R = 114,
	KEY_X = 120,
	KEY_Z = 122,
	KEY_ENTER = 13,
	KEY_BSP = 8,
	KEY_ESC = 27,
	EXTEND_KEY = 224,
	KEY_UP = 72,
	KEY_DOWN = 80,
	KEY_LEFT = 75,
	KEY_RIGHT = 77
};
int menu(void);
void drawMenuBorder(int x, int y);
void drawBigMenuBorder(int x, int y);
int menu_init(void);
void menu_options(void);
int get_ref_speed(void);
void set_ref_speed(int speed);
/*
	-- void show_select_menu(int x, int y, int *cursor, char str[][MAX_ARRAY_SIZE], int col, int row);
	x, y: 기준점 좌표
	cursor: cursor로 사용할 int 변수의 주소
	str: 메뉴로 사용할 배열
	col: 메뉴 영역의 가로 길이 (화면 지우기에 사용됨)
	row: sizeof로 계산한 배열의 세로 길이 (화면 지우기에 사용됨)
*/
void show_select_menu(int x, int y, int *cursor, char str[][MAX_ARRAY_SIZE], int col, int row);
void menu_pause(void);
int keyInput(int *cursor, int min_pos, int max_pos);

//character.c
#define MAX_LEVEL 10
typedef struct _LevelupRequiredExp {
	int level;
	int required_exp;
	int max_hp;
	int max_tp;
	int p_attack;
	int p_defense;
}USER_LEVEL;

struct _UserInfo {
	int account_id;
	char username[24]; //닉네임
	int level; //레벨
	int user_exp; //경험치
	int hp; //현재 체력
	int max_hp; //최대 체력
	int tp; //현재 TP
	int max_tp; //최대 TP
	int p_attack; //물리 공격력
	int p_defense; //물리 방어력
	int m_attack; //마법 공격력
	int m_defense; //마법 방어력

	int map_id; //플레이어가 있는 맵의 ID
	int map_x; //플레이어가 있는 좌표
	int map_y; //플레이어가 있는 좌표

	int gold; //골드
	USER_LEVEL user_level[MAX_LEVEL]; //레벨 당 필요 경험치
}user;

//struct _CurrentUser user;
int get_max_level(void);
void show_status_init(int hp_pos_X, int hp_pos_Y, int tp_pos_X, int tp_pos_Y);
void show_status(int show_lvl_name, int is_grayscale_mode);
void show_character_info(void);


//enemies.c
struct _EnemyInfo {
	int enemy_id;
	//char *name;
	char name[MAX_ARRAY_SIZE];
	int level;
	int hp;
	int max_hp;
	int p_attack;
	int p_defense;
	int m_attack;
	int m_defense;
	int boss_flag;
};

typedef struct _CurrentEnemyInfo {
	int enemy_id;
	//char *name;
	char name[MAX_ARRAY_SIZE];
	int level;
	int hp;
	int max_hp;
	int p_attack;
	int p_defense;
	int m_attack;
	int m_defense;
	int boss_flag;
	int active;
}CurrentEnemies;

#define MAX_ENEMIES 30
void show_enemy_status_init(int hp_pos_X, int hp_pos_Y);
void show_enemy_status(int enemy_id, int is_grayscale_mode);
int getEnemyInfo(void);
int SmartFetchEnemies(int enemy_id);
int setCurrentEnemies(int enemy1_id, int enemy2_id, int enemy3_id);
int freeEnemyInfo(void);
int freeCurrentEnemies(void);

//skills.c
struct _PlayerSkillList {
	int skill_id;
	//char *name;
	char name[MAX_ARRAY_SIZE];
	int level;
	int p_damage;
	int m_damage;
	int(*skill[5])();
};

struct _EnemySkillList {
	int skill_id;
	char name[MAX_ARRAY_SIZE];
	int level;
	int p_damage;
	int m_damage;
	int(*skill[5])();
};

int usePlayerSkill(void);

//item.c
/*#define MAX_ITEM 256
struct _item {
	int id;
	char *name;
	char *type;
	char *rare;
	int max_level;
	int p_attack;
	int p_defense;
	int m_attack;
	int m_defense;
};

int getItemInfo(void);
*/

//battle.c
enum _AttackPosition {
	AP_PtE, //Player to Enemy
	AP_EtP //Enemy to Player
};
void ResetEscapeClickFlag(void);
int AttackProcess(int mode);
int hit_damage(int *target, int damage);
void tp_up(int damage);
void game_over(void);
void battle_result(void);
void lvlup_check(void);
void showEnemyList(void);
int keyInput_selectEnemy(int *cursor, int min_pos, int max_pos);
void select_enemy(int *cursor, int enemyCount);
int battle_screen(int enemyCount);

//map.c
struct MAP {
	int map_id;
	char *map_name;
	struct MAP_LEVEL *level[6][6];
	char const (*_level)[14];
	char(*_level_disp)[14];
	int const (*_level_mask)[6];
};

struct MAP_LEVEL {
	int is_first;
	int mask;
};

enum map_position_move_mode {
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_AUTO
};
void map_init(void);
void map_clean(void);
void chk_dispMap(void);
void show_current_map(void);
void move_anim(void);

//dungeon.c
#define MAPID user.map_id
#define USERX user.map_x
#define USERY user.map_y
int d_EventInit(void);
int d_randomEvent(void);
void d_movePos(int mode);

int _default(void);
int _battle(void);
int _item(void);
int _up(void);
int _down(void);
int _special(void);