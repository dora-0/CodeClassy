#include <stdio.h>
#include <process.h>
#include "main.h"
#define MAP_POS_X WIDTH_SIZE - 15
#define MAP_POS_Y 2
struct MAP *map[6];

/*
	** map.c : 맵 관리

	실행 순서:
	map_init()
	Loop:
	(위치 변경)
	-> move_position(int mode)
	-> move_anim();
	(맵 표시)
	-> 외부에서 show_current_map() 호출
	-> show_current_map() 함수에 의해 chk_dispMap() 호출됨
	-> 실행: chk_dispMap() -> show_current_map()
	:END
	-> map_clean();
	
	1) struct MAP
	** Map의 이름, 레벨 정보 등 맵 데이터를 담을 구조체
	사용할 땐 1차원 구조체 배열 형태로 선언한다.

	map_id : 고유 ID
	map_name : 맵의 이름 (출력에 사용됨)
	level : MAP_LEVEL 구조체 배열
	_level : 전체 맵 구조
	_level_disp : 사용자에게 보여질 맵 구조
	_level_mask : 각 방의 특수 트리거 정보

	===

	2) struct MAP_LEVEL
	** Map 내 각 방의 속성 데이터를 담을 구조체
	사용할 땐 2차원 구조체 배열 6*6으로 선언한다.

	is_first : 방에 처음으로 접근한 것인지의 여부.
	is_blocked : 방에 접근이 가능한 지의 여부.

	방에 접근할 경우 is_first 값이 0이 되며, map_lvXXX_disp 배열(->사용자 출력)에 해당 방의 위치가 기록된다.

	===

	3) static char const map_lvXXX[6][14]
	** Map의 전체 맵 구조를 담은 2차원 배열

	맵의 크기는 6*6이나, 맵의 방을 표현하는데 2바이트 문자를 사용하므로
	가로 길이는 6 * 2 + 2 = 14로 정의하였다. (2바이트 문자 6개(12) + '\0' + 예비 공간 1바이트)

	===

	4) static char map_lvXXX_disp[6][14]
	** Map의 사용자 출력 데이터를 담은 2차원 배열

	map_lvXXX 배열과 비슷하나 이 배열의 데이터는 비어있는 상태로
	사용자가 방에 처음 접근하면 방의 위치가 기록되도록 설계하였다.

	즉, 처음에는 맵에 아무것도 나타나지 않다가
	새로운 방을 탐색할수록 맵의 구조가 기록되는 형태이다.

	===

	5) static int const map_lvXXX_mask[6][14]
	** Map "LevelXXX"의 각 방의 특수 데이터를 정수값으로 담은 2차원 배열

	이 배열에 정의된 데이터가
	map_init() 함수에서 참조되어 각 방의 is_blocked 변수로 입력된다.

	-1 : 이벤트 없음
	0 : 전투 이벤트
	1 : 이동 불가
	7 : 특수 이벤트
	8 : 계단 (Up)
	9 : 계단 (Down)

	===

	6) void map_init(void)
	** 맵 구조체를 초기화하는 함수

	맵 구조체에 메모리를 할당하고, map_lvXXX_mask 배열을 참조해 각 방의 특수 데이터 값을 설정한다.
	사용자(구조체 user)의 현재 좌표 값을 각 방의 시작점 값으로 설정한다.

	map_clean() 함수 실행 시, map_lvXXX_disp 배열의 데이터가 모두 지워지는데
	해당 함수 실행 후 맵이 다시 초기화되고 첫 이동 시 원래 있던 자리에는
	방의 위치가 기록되지 않으므로 해당 위치의 is_first 값을 0으로 설정한다.

	map_lvXXX_disp 배열의 데이터가 저장된 파일이 있을 경우 해당 파일을 로드한다. (map_clean()에서 저장된 데이터)	

	===

	7) void map_clean(void)
	** 맵 구조체에 할당된 메모리를 해제하는 함수

	맵 구조체에 할당된 메모리를 해제하고, map_lvXXX_disp 배열의 데이터를 비운다.

	추가로 map_lvXXX_disp 배열의 데이터를 로컬 데이터로 저장한다.
	(이미 가봤던 장소는 게임 종료 후 재시작 후에도 확인할 수 있도록)

	===

	8) void chk_dispMap(void)
	** 각 방의 첫 접근 여부를 검사해 map_lvXXX_disp 배열에 기록하는 함수

	맵 구조체에서 각 방의 is_first 값이 0이라면
	map_lvXXX 배열에서 방과 같은 위치의 데이터를 map_lvXXX_disp 배열의 같은 위치로 복사한다.

	즉, 사용자 입장에서 한번 방문한 방의 위치가 보이게 된다.

	===

	9) void show_current_map(void)
	** 현재 맵 정보를 map_lvXXX_disp 배열을 참조해 보여주는 함수

	먼저 chk_dispMap() 함수를 실행한 후,
	상수 MAP_POS_X, MAP_POS_Y 좌표로 이동해 map_lvXXX_disp 배열의 내용을 보여준다.

	현재 위치한 곳은 붉은색 네모로 표시해 강조 효과를 준다.

	===

	11) void move_anim(void)
	** 방 이동 애니메이션 재생 함수
	system 함수와 Sleep 함수 등으로 페이드 아웃-페이드 인 애니메이션을 구현한 함수이다.
	move_position() 함수에서 자동으로 호출된다.

*/

static char const map_lv001[6][14] = {
	"            ",
	"  ■        ",
	"  □  ▨    ",
	"  □□□□  ",
	"    □      ",
	"            "
};

static char const map_lv002[6][14] = {
	"            ",
	"  □        ",
	"  □□▧    ",
	"  □    □  ",
	"  □□□□  ",
	"            "
};

static char map_lv001_disp[6][14] = {
	"            ",
	"  ■        ",
	"            ",
	"            ",
	"            ",
	"            "
};


static char map_lv002_disp[6][14] = {
	"            ",
	"            ",
	"      ▧    ",
	"            ",
	"            ",
	"            "
};

/*
	-1 : none (open)
	0 : random event
	1 : blocked
	7 : special event
	8 : staircase (Up)
	9 : staircase (Down)
*/

static int const map_lv001_mask[6][6] = {
	{ 1, 1, 1, 1, 1, 1 },
	{ 1,-1, 1, 1, 1, 1 },
	{ 1, 0, 1, 8, 1, 1 },
	{ 1, 0, 0, 0, 0, 1 },
	{ 1, 1, 7, 1, 1, 1 },
	{ 1, 1, 1, 1, 1, 1 }
};

static int const map_lv002_mask[6][6] = {
	{ 1, 1, 1, 1, 1, 1 },
	{ 1, 0, 1, 1, 1, 1 },
	{ 1, 0, 0, 9, 1, 1 },
	{ 1, 0, 1, 1, 0, 1 },
	{ 1, 0, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 1, 1 }
};

void map_init(void) {
	int i = 0, j = 0, k = 0;

	for (i = 0; i < 6; i++) {
		//map에 메모리 할당
		map[i] = malloc(sizeof(struct MAP));
		map[i]->map_id = i;

		for (j = 0; j < 6; j++) {
			for (k = 0; k < 6; k++) {
				//map->level에 메모리 할당
				map[i]->level[j][k] = malloc(sizeof(struct MAP_LEVEL));
				map[i]->level[j][k]->is_first = 1;
			}
		}
	}

	FILE *fp;
	fopen_s(&fp, "map_data.bin", "r+");

	if (fp != NULL) {
		fread(&map_lv001_disp, sizeof(map_lv001_disp), 1, fp);
		fread(&map_lv002_disp, sizeof(map_lv002_disp), 1, fp);
		fclose(fp);
	}
	

	//맵 이름 및 데이터 설정
	map[0]->map_name = "1층";
	map[0]->_level = map_lv001;
	map[0]->_level_disp = map_lv001_disp;
	map[0]->_level_mask = map_lv001_mask;
	map[0]->level[1][1]->is_first = 0;

	map[1]->map_name = "2층";
	map[1]->_level = map_lv002;
	map[1]->_level_disp = map_lv002_disp;
	map[1]->_level_mask = map_lv002_mask;
	map[1]->level[2][3]->is_first = 0;

	//mask
	for (i = 0; i < 6; i++) {
		for (j = 0; j < 6; j++) {
			map[0]->level[i][j]->mask = map[0]->_level_mask[i][j];
			map[1]->level[i][j]->mask = map[1]->_level_mask[i][j];
		}
	}
}

void map_clean(void) {
	int i = 0, j = 0, k = 0;
	FILE *fp;
	fopen_s(&fp, "map_data.bin", "wb");
	fwrite(&map_lv001_disp, sizeof(map_lv001_disp), 1, fp);
	fwrite(&map_lv002_disp, sizeof(map_lv002_disp), 1, fp);
	fclose(fp);

	//메모리 해제
	for (i = 0; i < 6; i++) {
		for (j = 0; j < 6; j++) {
			for (k = 0; k < 6; k++) {
				free(map[i]->level[j][k]);
			}
		}
		free(map[i]);
	}
}

void chk_dispMap(void) {
	int i = 0, j = 0;

	for (i = 0; i < 6; i++) {
		for (j = 0; j < 6; j++) {
			if (map[MAPID]->level[i][j]->is_first == 0) {
				//j * 2인 이유는 문자가 2바이트이므로
				map[MAPID]->_level_disp[i][j * 2] = map[MAPID]->_level[i][j * 2];
				map[MAPID]->_level_disp[i][j * 2 + 1] = map[MAPID]->_level[i][j * 2 + 1];
			}
		}
	}
}

void show_current_map(void) {
	int i = 0, j = 0;
	int row = 6; //배열의 세로 크기
	
	//is_first 검사
	chk_dispMap();
	gotoxy(MAP_POS_X, MAP_POS_Y - 1);
	//맵 이름 출력
	printf("== %s ==", map[MAPID]->map_name);
	
	gotoxy(MAP_POS_X, MAP_POS_Y);
	for (i = 0; i < row; i++) {
		gotoxy(MAP_POS_X, MAP_POS_Y + i);
		for (j = 0; j < 14; j++) {
			//네모는 2바이트 문자이므로 2번씩 출력
			printf("%c%c", map[MAPID]->_level_disp[i][j], map[MAPID]->_level_disp[i][j + 1]);
			j++;
		}
	}

	gotoxy(MAP_POS_X + (USERX) * 2, MAP_POS_Y + (USERY));
	setFontColor(CC_RED);
	printf("■");
	setFontColor(CC_WHITE);
}

void move_anim(void) {
	system("color 08");
	Sleep(200);
	system("cls");
	Sleep(500);
	show_status_init(1, HEIGHT_SIZE - 2, 26, HEIGHT_SIZE - 2);
	show_status(1, 1);
	show_current_map();
	Sleep(200);
	show_status(1, 0);
}