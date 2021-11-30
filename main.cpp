#include <stdio.h>
#include <easyx.h>
#include <time.h>
#include <mmsystem.h>
#include "resource.h"
#pragma comment(lib,"WINMM.LIB")

// 整体窗口尺寸
#define WIDTH 288
#define HEIGHT 512
#define PIPE_WIDTH 52 // 管道宽度
#define PIPE_HEIGHT 320 // 管道高度
#define BIRD_SIZE 48 // 小鸟尺寸
#define GAMEOVER_WIDTH 204
#define GAMEOVER_HEIGHT 54
#define GAMESTART_WIDTH 114
#define GAMESTART_HEIGHT 98
#define GAMETITLE_WIDTH 178
#define GAMETILE_HEIGHT 48
#define BUTTON_PLAY_WIDTH 116
#define BUTTON_PLAY_HEIGHT 70
#define UNIFORM_SPACE 10 // 统一间隔标准

unsigned long tc1, tc2; // 定义两个时间 ms
unsigned int tcDiff = 30; // 更新时间差 降频操作
unsigned int isRun = 0; // 是否正在运行 
unsigned int isFailed = 0;
unsigned pipeSpace = 190; // 两根管道之间的间隔
unsigned pipeRise = -280;  // 管道上升和下沉
unsigned pipeDrop = 140;
IMAGE backImg; // 定义背景图片变量
struct Land { // 地面结构
	int x = 0, y = 0;	// 地面坐标
	int xSpeed = 0;	// 横向移动速度
	IMAGE img;		// 地面图片 images/land.png 336*112
} land;
struct Bird { // 小鸟结构
	int x = 0, y = 0;	// 坐标
	int ySpeed = 0; // 垂直速度
	int mockG = 0;	// 重力加速度模拟数据
	int frame = 0;	// 帧
	IMAGE img[3][2]; // 图片 48*48
} bird;
struct Pipe {
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	int xSpeed = 0;
	IMAGE img[2];
} pipe;

struct GameTitle {
	int x = 0, y = 0;
	IMAGE img; // images/title.png 178*48
} gameTitle;

struct GameStart {
	int x = 0, y = 0;
	IMAGE img; // images/tutorial.png 114*94
} gameStart;

struct GameBtnPlay {
	int x = 0, y = 0;
	IMAGE img; // images/button_play.png 116*70
} gameBtnPlay;

struct GameOver {
	int x = 0, y = 0;
	int ySpeed = 0;
	IMAGE img; // images/text_game_over.png 204*54
} gameOver;

void GameInit() {
	// 初始化背景音乐
	//mciSendString("open images/bg.mp3 alias song", 0, 0, 0);
	//mciSendString("play song repeat", 0, 0, 0);

	PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_RESOURCE | SND_LOOP | SND_ASYNC);

	srand(time(0)); // 初始化时间种子
	isFailed = 0;
	isRun = 0;

	// 初始化窗口
	initgraph(WIDTH, HEIGHT);
	// 初始化背景图片
	loadimage(&backImg, "PNG", MAKEINTRESOURCE(IDB_PNG1));
	// 初始化地面结构
	land.x = 0;
	land.y = 420;
	land.xSpeed = 2;
	loadimage(&land.img, "PNG", MAKEINTRESOURCE(IDB_PNG9));
	// 初始化定时器
	tc1 = GetTickCount();
	tc2 = GetTickCount();
	// 初始化小鸟
	bird.x = 20;
	bird.y = 200;
	bird.ySpeed = 0;
	bird.mockG = 1; // 单位时间的加速度
	bird.frame = 0;
	loadimage(&bird.img[0][0], "PNG", MAKEINTRESOURCE(IDB_PNG3));
	loadimage(&bird.img[0][1], "PNG", MAKEINTRESOURCE(IDB_PNG2));
	loadimage(&bird.img[1][0], "PNG", MAKEINTRESOURCE(IDB_PNG5));
	loadimage(&bird.img[1][1], "PNG", MAKEINTRESOURCE(IDB_PNG6));
	loadimage(&bird.img[2][0], "PNG", MAKEINTRESOURCE(IDB_PNG4));
	loadimage(&bird.img[2][1], "PNG", MAKEINTRESOURCE(IDB_PNG7));
	// 初始化管道
	pipe.x1 = WIDTH;
	pipe.y1 = rand() % 250;
	pipe.x2 = WIDTH + pipeSpace;
	pipe.y2 = rand() % 250;
	pipe.xSpeed = 2;
	loadimage(&pipe.img[0], "PNG", MAKEINTRESOURCE(IDB_PNG10));
	loadimage(&pipe.img[1], "PNG", MAKEINTRESOURCE(IDB_PNG11));
	// 初始化游戏名称
	gameTitle.x = (WIDTH - GAMETITLE_WIDTH) / 2;
	gameTitle.y = (HEIGHT - GAMETILE_HEIGHT) / 2 - 8 * UNIFORM_SPACE - (HEIGHT - land.y);
	loadimage(&gameTitle.img, "PNG", MAKEINTRESOURCE(IDB_PNG14));
	// 初始化游戏开始
	gameStart.x = (WIDTH - GAMESTART_WIDTH) / 2;
	gameStart.y = gameTitle.y + 2 * UNIFORM_SPACE + GAMETILE_HEIGHT;
	loadimage(&gameStart.img, "PNG", MAKEINTRESOURCE(IDB_PNG15));

	// 初始化开始按钮
	gameBtnPlay.x = (WIDTH - BUTTON_PLAY_WIDTH) / 2;
	gameBtnPlay.y = gameStart.y + UNIFORM_SPACE + GAMESTART_HEIGHT;
	loadimage(&gameBtnPlay.img, "PNG", MAKEINTRESOURCE(IDB_PNG8));

	// 初始化游戏结束
	gameOver.x = (WIDTH - GAMEOVER_WIDTH) / 2;
	gameOver.y = (HEIGHT - GAMEOVER_HEIGHT) / 2;
	gameOver.ySpeed = 2;
	loadimage(&gameOver.img, "PNG", MAKEINTRESOURCE(IDB_PNG12));
}

void GameDraw() {
	BeginBatchDraw(); // 开始绘制

	// 绘制背景图片
	putimage(0, 0, &backImg);
	// 绘制管道
	putimage(pipe.x1, pipe.y1 + pipeRise, &pipe.img[0]);
	putimage(pipe.x1, pipe.y1 + pipeDrop, &pipe.img[1]);
	putimage(pipe.x2, pipe.y2 + pipeRise, &pipe.img[0]);
	putimage(pipe.x2, pipe.y2 + pipeDrop, &pipe.img[1]);
	// 绘制地面
	putimage(land.x, land.y, &land.img);

	if (isRun) {
		// 绘制小鸟
		putimage(bird.x, bird.y, &bird.img[bird.frame][0], SRCPAINT);
		putimage(bird.x, bird.y, &bird.img[bird.frame][1], SRCPAINT);
		// 绘制结束
		if (isFailed) putimage(gameOver.x, gameOver.y, &gameOver.img, SRCPAINT);
	}
	else {
		putimage(gameTitle.x, gameTitle.y, &gameTitle.img, SRCPAINT); // 绘制名称
		putimage(gameStart.x, gameStart.y, &gameStart.img, SRCPAINT); // 绘制开始
		putimage(gameBtnPlay.x, gameBtnPlay.y, &gameBtnPlay.img, SRCPAINT); // 绘制开始按钮
	}

	EndBatchDraw(); // 结束绘制
}

void GameUpdate() {

	// 手动更新，人为操作引起的数据变化
	ExMessage msg = { 0 }; // MouseMsg & MouseHit已弃用
	if (peekmessage(&msg, EM_MOUSE)) {
		if (msg.message == WM_LBUTTONDOWN) { // 鼠标左键
			if (isRun) {
				if (!isFailed) bird.ySpeed = -10;
			}
			else {
				if (
					msg.x >= gameBtnPlay.x && msg.x <= (gameBtnPlay.x + BUTTON_PLAY_WIDTH) &&
					msg.y >= gameBtnPlay.y && msg.y <= (gameBtnPlay.y + BUTTON_PLAY_HEIGHT)
					) {
					isRun = 1;
				}
			}

		}
		else if (msg.message == WM_RBUTTONDOWN) { // 鼠标右键

		}
	}

	// 自动更新，即不需要人为控制
	tc2 = GetTickCount();
	if (tc2 - tc1 > tcDiff) {
		// 地面左移 x轴-速度
		land.x -= land.xSpeed;
		if (land.x < -20) land.x = 0;
		// 更新小鸟
		// 帧数更新
		if (++bird.frame >= 3) bird.frame = 0;
		if (!isFailed && isRun) {

			// 自由落体 Y轴
			bird.ySpeed += bird.mockG;
			bird.y += bird.ySpeed;

			// 更新管道
			pipe.x1 -= pipe.xSpeed;
			pipe.x2 -= pipe.xSpeed;
			if (pipe.x1 < -PIPE_WIDTH) pipe.x1 = pipe.x2 + pipeSpace;
			if (pipe.x2 < -PIPE_WIDTH) pipe.x2 = pipe.x1 + pipeSpace;

		}
		else {
			if (isFailed) {
				gameOver.y -= gameOver.ySpeed;
				if (gameOver.y <= -GAMEOVER_HEIGHT) {
					GameInit();
				}
			}
		}

		tc1 = tc2;
	}

	// 管道一 (bird.x >= pipe.x1 && (bird.x+BIRD_SIZE) <= (pipe.x1+PIPE_WIDTH)) && (bird.y <= (pipe.y1 + pipeRise) || (bird.y+BIRD_SIZE) >= (pipe.y1 + pipeDrop))
	// 管道二 (bird.x >= pipe.x2 && (bird.x+BIRD_SIZE) <= (pipe.x2+PIPE_WIDTH)) && (bird.y <= (pipe.y2 + pipeRise) || (bird.y+BIRD_SIZE) >= (pipe.y2 + pipeDrop))
	// 落地 (bird.y+BIRD_SIZE) >= land.y
	if (
		!isFailed && isRun &&
		(
			((bird.x >= pipe.x1 && (bird.x + BIRD_SIZE) <= (pipe.x1 + PIPE_WIDTH)) && (bird.y <= (PIPE_HEIGHT + pipe.y1 + (int)pipeRise) || (bird.y + BIRD_SIZE) >= (pipe.y1 + (int)pipeDrop))) ||
			((bird.x >= pipe.x2 && (bird.x + BIRD_SIZE) <= (pipe.x2 + PIPE_WIDTH)) && (bird.y <= (PIPE_HEIGHT + pipe.y2 + (int)pipeRise) || (bird.y + BIRD_SIZE) >= (pipe.y2 + (int)pipeDrop))) ||
			(bird.y + BIRD_SIZE) >= land.y
			)
		) {
		isFailed = 1;
	}
}

int main() {
	GameInit();

	while (1) {
		GameDraw();
		GameUpdate();
	}
	closegraph();

	return 0;
}