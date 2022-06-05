#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<bangtal.h>


#define ROW 10
#define COL 10
#define MineNum 10

int map[ROW][COL] = { 0 };
SceneID room;
ObjectID startButton, endButton, minesweeper;
ObjectID picture[100];

ObjectID createObject(const char* image, SceneID scene, int x, int y, bool show) {
	ObjectID object = createObject(image);
	locateObject(object, scene, x, y);

	if (show == true) {
		showObject(object);
	}
	return object;
}

//初始化，生成二维数组并全是0
void Show(int map[ROW][COL]) {
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			printf("%d ", map[i][k]);
		}
		putchar('\n');
	}
}

//随机生成10个雷，用-1表示
void init(int map[ROW][COL]) {
	for (int i = 0; i < MineNum; i++) {
		int r = rand() % 10;
		int c = rand() % 10;
		if (map[r][c] == 0) {//防止重复
			map[r][c] = -1;
		}
		else {
			i--;
		}
	}
	//以雷为中心的九宫格 +1 雷除外
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			//找雷
			if (map[i][k] == -1) {
				for (int r = i - 1; r <= i + 1; r++) {
					for (int c = k - 1; c <= k + 1; c++) {
						if ((r >= 0 && r < ROW && c >= 0 && c < COL) && (map[r][c] != -1)) {
							map[r][c]++;
						}
					}
				}
			}
		}
	}

	//加密
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			map[i][k] += 20;
		}
	}
}

//换对应数字和炸弹
void draw(int map[ROW][COL]) {
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			//先数字
			if (map[i][k] >= 0 && map[i][k] <= 8) {
				char filename[25];
				sprintf_s(filename, "%d.png",map[i][k]);
				picture[i * 10 + k] = createObject(filename, room, 350 + k * 50, 100 + i * 50, true);
				scaleObject(picture[i * 10 + k], 0.5f);
			}
			else if (map[i][k] == -1) {
				picture[i * 10 + k] = createObject("-1.png", room, 350 + k* 50, 100 + i * 50, true);
				scaleObject(picture[i * 10 + k], 0.5f);
			}
			else if (map[i][k] >= 19 && map[i][k] <= 28) {
				picture[i * 10 + k] = createObject("11.png", room, 350 + k * 50, 100 + i * 50, true);
				scaleObject(picture[i * 10 + k], 0.5f);
			}
		}
	}
}

void startGame() {

	srand((unsigned)time(NULL));
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			map[i][k] = 0;
		}
	}

	init(map);
	Show(map);
	draw(map);

	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			picture[i * 10 + k] = createObject("11.png", room, 350 + k * 50, 100 + i * 50, true);
			scaleObject(picture[i * 10 + k], 0.5f);
		}
	}
}

int board_index(ObjectID object) {
	for (int i = 0; i < 100; i++) {
		if (picture[i] == object)return i;
	}
	return -1;
}

//空格
void BoomBlank(int map[ROW][COL],int x,int y) {
	if (map[x][y] == 0) {
		for (int n = x - 1; n <= x + 1; n++) {
			for (int m = y - 1; m <= y + 1; m++) {
				//注意越界
				if ((n >= 0 && n < ROW && m >= 0 && m < COL) && (map[n][m] > 19 && map[n][m] <= 28)) {
					map[n][m] -= 20;
					BoomBlank(map, n,m);
				}
			}
		}
	}
}

//胜利 1，输 -1，没结束0
int judge(int map[ROW][COL], int row, int col) {
	if (map[row][col] == -1 || map[row][col] == 19) {
		return -1;
	}

	int cnt = 0;
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			if (map[i][k] <=8) {
				cnt++;
			}
		}
	}
	if (cnt == 90) {
		return 1;
	}
	return 0;
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action) {
	int index = board_index(object);

	if (object == startButton) {
		startGame();
		setObjectImage(startButton, "restart.png");
	}
	else if (object == endButton) {
		endGame();
	}
	else if (object == picture[index]) {
			int r, c;
			c = index % 10;
			r = index / 10;
			map[r][c] -= 20;
			BoomBlank(map, r, c);
			int b = judge(map, r, c);
			draw(map);
			if (b == -1) {
				showMessage("실배!");
				for (int i = 0; i < ROW; i++) {
					for (int k = 0; k < COL; k++) {
						if (map[i][k] == -1 || map[i][k] == 19) {
							picture[i * 10 + k] = createObject("-1.png", room, 350 + k * 50, 100 + i * 50, true);
							scaleObject(picture[i * 10 + k], 0.5f);
						}
					}
				}
			}
			else if (b == 1) {
				showMessage("성공!");
			}
	}
}

int main(void) {

	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	setMouseCallback(mouseCallback);

	room = createScene("room");
	minesweeper = createObject("minesweeper.jpg", room, 350, 100, true);
	startButton = createObject("start.png", room, 900, 350, true);
	endButton = createObject("end.png", room, 900, 300, true);

	srand((unsigned)time(NULL));
	
	startGame(room);

	return 0;	
}
