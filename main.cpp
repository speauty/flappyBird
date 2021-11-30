#include <stdio.h>
#include <easyx.h>
#include <time.h>
#include <mmsystem.h>
#include "resource.h"
#pragma comment(lib,"WINMM.LIB")

// ���崰�ڳߴ�
#define WIDTH 288
#define HEIGHT 512
#define PIPE_WIDTH 52 // �ܵ����
#define PIPE_HEIGHT 320 // �ܵ��߶�
#define BIRD_SIZE 48 // С��ߴ�
#define GAMEOVER_WIDTH 204
#define GAMEOVER_HEIGHT 54
#define GAMESTART_WIDTH 114
#define GAMESTART_HEIGHT 98
#define GAMETITLE_WIDTH 178
#define GAMETILE_HEIGHT 48
#define BUTTON_PLAY_WIDTH 116
#define BUTTON_PLAY_HEIGHT 70
#define UNIFORM_SPACE 10 // ͳһ�����׼

unsigned long tc1, tc2; // ��������ʱ�� ms
unsigned int tcDiff = 30; // ����ʱ��� ��Ƶ����
unsigned int isRun = 0; // �Ƿ��������� 
unsigned int isFailed = 0;
unsigned pipeSpace = 190; // �����ܵ�֮��ļ��
unsigned pipeRise = -280;  // �ܵ��������³�
unsigned pipeDrop = 140;
IMAGE backImg; // ���屳��ͼƬ����
struct Land { // ����ṹ
	int x = 0, y = 0;	// ��������
	int xSpeed = 0;	// �����ƶ��ٶ�
	IMAGE img;		// ����ͼƬ images/land.png 336*112
} land;
struct Bird { // С��ṹ
	int x = 0, y = 0;	// ����
	int ySpeed = 0; // ��ֱ�ٶ�
	int mockG = 0;	// �������ٶ�ģ������
	int frame = 0;	// ֡
	IMAGE img[3][2]; // ͼƬ 48*48
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
	// ��ʼ����������
	//mciSendString("open images/bg.mp3 alias song", 0, 0, 0);
	//mciSendString("play song repeat", 0, 0, 0);

	PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_RESOURCE | SND_LOOP | SND_ASYNC);

	srand(time(0)); // ��ʼ��ʱ������
	isFailed = 0;
	isRun = 0;

	// ��ʼ������
	initgraph(WIDTH, HEIGHT);
	// ��ʼ������ͼƬ
	loadimage(&backImg, "PNG", MAKEINTRESOURCE(IDB_PNG1));
	// ��ʼ������ṹ
	land.x = 0;
	land.y = 420;
	land.xSpeed = 2;
	loadimage(&land.img, "PNG", MAKEINTRESOURCE(IDB_PNG9));
	// ��ʼ����ʱ��
	tc1 = GetTickCount();
	tc2 = GetTickCount();
	// ��ʼ��С��
	bird.x = 20;
	bird.y = 200;
	bird.ySpeed = 0;
	bird.mockG = 1; // ��λʱ��ļ��ٶ�
	bird.frame = 0;
	loadimage(&bird.img[0][0], "PNG", MAKEINTRESOURCE(IDB_PNG3));
	loadimage(&bird.img[0][1], "PNG", MAKEINTRESOURCE(IDB_PNG2));
	loadimage(&bird.img[1][0], "PNG", MAKEINTRESOURCE(IDB_PNG5));
	loadimage(&bird.img[1][1], "PNG", MAKEINTRESOURCE(IDB_PNG6));
	loadimage(&bird.img[2][0], "PNG", MAKEINTRESOURCE(IDB_PNG4));
	loadimage(&bird.img[2][1], "PNG", MAKEINTRESOURCE(IDB_PNG7));
	// ��ʼ���ܵ�
	pipe.x1 = WIDTH;
	pipe.y1 = rand() % 250;
	pipe.x2 = WIDTH + pipeSpace;
	pipe.y2 = rand() % 250;
	pipe.xSpeed = 2;
	loadimage(&pipe.img[0], "PNG", MAKEINTRESOURCE(IDB_PNG10));
	loadimage(&pipe.img[1], "PNG", MAKEINTRESOURCE(IDB_PNG11));
	// ��ʼ����Ϸ����
	gameTitle.x = (WIDTH - GAMETITLE_WIDTH) / 2;
	gameTitle.y = (HEIGHT - GAMETILE_HEIGHT) / 2 - 8 * UNIFORM_SPACE - (HEIGHT - land.y);
	loadimage(&gameTitle.img, "PNG", MAKEINTRESOURCE(IDB_PNG14));
	// ��ʼ����Ϸ��ʼ
	gameStart.x = (WIDTH - GAMESTART_WIDTH) / 2;
	gameStart.y = gameTitle.y + 2 * UNIFORM_SPACE + GAMETILE_HEIGHT;
	loadimage(&gameStart.img, "PNG", MAKEINTRESOURCE(IDB_PNG15));

	// ��ʼ����ʼ��ť
	gameBtnPlay.x = (WIDTH - BUTTON_PLAY_WIDTH) / 2;
	gameBtnPlay.y = gameStart.y + UNIFORM_SPACE + GAMESTART_HEIGHT;
	loadimage(&gameBtnPlay.img, "PNG", MAKEINTRESOURCE(IDB_PNG8));

	// ��ʼ����Ϸ����
	gameOver.x = (WIDTH - GAMEOVER_WIDTH) / 2;
	gameOver.y = (HEIGHT - GAMEOVER_HEIGHT) / 2;
	gameOver.ySpeed = 2;
	loadimage(&gameOver.img, "PNG", MAKEINTRESOURCE(IDB_PNG12));
}

void GameDraw() {
	BeginBatchDraw(); // ��ʼ����

	// ���Ʊ���ͼƬ
	putimage(0, 0, &backImg);
	// ���ƹܵ�
	putimage(pipe.x1, pipe.y1 + pipeRise, &pipe.img[0]);
	putimage(pipe.x1, pipe.y1 + pipeDrop, &pipe.img[1]);
	putimage(pipe.x2, pipe.y2 + pipeRise, &pipe.img[0]);
	putimage(pipe.x2, pipe.y2 + pipeDrop, &pipe.img[1]);
	// ���Ƶ���
	putimage(land.x, land.y, &land.img);

	if (isRun) {
		// ����С��
		putimage(bird.x, bird.y, &bird.img[bird.frame][0], SRCPAINT);
		putimage(bird.x, bird.y, &bird.img[bird.frame][1], SRCPAINT);
		// ���ƽ���
		if (isFailed) putimage(gameOver.x, gameOver.y, &gameOver.img, SRCPAINT);
	}
	else {
		putimage(gameTitle.x, gameTitle.y, &gameTitle.img, SRCPAINT); // ��������
		putimage(gameStart.x, gameStart.y, &gameStart.img, SRCPAINT); // ���ƿ�ʼ
		putimage(gameBtnPlay.x, gameBtnPlay.y, &gameBtnPlay.img, SRCPAINT); // ���ƿ�ʼ��ť
	}

	EndBatchDraw(); // ��������
}

void GameUpdate() {

	// �ֶ����£���Ϊ������������ݱ仯
	ExMessage msg = { 0 }; // MouseMsg & MouseHit������
	if (peekmessage(&msg, EM_MOUSE)) {
		if (msg.message == WM_LBUTTONDOWN) { // ������
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
		else if (msg.message == WM_RBUTTONDOWN) { // ����Ҽ�

		}
	}

	// �Զ����£�������Ҫ��Ϊ����
	tc2 = GetTickCount();
	if (tc2 - tc1 > tcDiff) {
		// �������� x��-�ٶ�
		land.x -= land.xSpeed;
		if (land.x < -20) land.x = 0;
		// ����С��
		// ֡������
		if (++bird.frame >= 3) bird.frame = 0;
		if (!isFailed && isRun) {

			// �������� Y��
			bird.ySpeed += bird.mockG;
			bird.y += bird.ySpeed;

			// ���¹ܵ�
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

	// �ܵ�һ (bird.x >= pipe.x1 && (bird.x+BIRD_SIZE) <= (pipe.x1+PIPE_WIDTH)) && (bird.y <= (pipe.y1 + pipeRise) || (bird.y+BIRD_SIZE) >= (pipe.y1 + pipeDrop))
	// �ܵ��� (bird.x >= pipe.x2 && (bird.x+BIRD_SIZE) <= (pipe.x2+PIPE_WIDTH)) && (bird.y <= (pipe.y2 + pipeRise) || (bird.y+BIRD_SIZE) >= (pipe.y2 + pipeDrop))
	// ��� (bird.y+BIRD_SIZE) >= land.y
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