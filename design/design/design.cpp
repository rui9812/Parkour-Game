#include "stdafx.h"
#include "graphics.h"
#include <conio.h>
#include <windows.h>
#include <time.h>
#include "mmsystem.h"
#pragma comment(lib, "WINMM.LIB")

#define  WIDTH 800
#define  HIGH 480			//宏定义画布尺寸
#define R_H 105
#define R_W 95				//定义跑动人物高与宽
#define T 700//图片显示延时

struct barrier //定义障碍物结构体
{
	int id;//代表障碍物种类,定义障碍物bar，为0代表无障碍物，1代表地刺，2代表下藤曼，3代表上藤蔓
	int begin_i;
	int pass_i;
	int end_i;
	int img_num;//显示障碍物的第img_num张图
	int i;//障碍物的横坐标
};

//定义全局变量
IMAGE img_gamestart_bg;//定义游戏开始界面背景
IMAGE img_bg;	// 定义 背景
IMAGE img_run;   //定义人物跑动图片
IMAGE img_run1;//跑动遮罩图
IMAGE img_jump;//人物跳跃图
IMAGE img_jump1;//跳跃遮罩图
IMAGE img_slide;//人物下滑图
IMAGE img_slide1;//下滑遮罩图
IMAGE img_trap;//地刺图
IMAGE img_trap1;//地刺遮罩图
IMAGE img_dowmPlant;//下藤曼图
IMAGE img_dowmPlant1;//下藤曼遮罩图
IMAGE img_upPlant;//上藤曼图
IMAGE img_upPlant1;//上藤曼遮罩图
IMAGE img_trapDeath;
IMAGE img_trapDeath1;//碰到地刺死亡的图片与遮罩图
IMAGE img_plantDeath;
IMAGE img_plantDeath1;//碰到藤曼死亡的图片和遮罩图
IMAGE img_deathbk;//死亡后 游戏背景界面
int img_bg_i;//背景图片分割线
int M_X;
int M_Y;//定义人物位置
int begin_i, end_i, pass_i;//begin_i为人物在图片上的初始像素，end_i为在图片上的当前像素，pass_i为通过图片像素
int img_num1;//显示第img_num1张跑动图片
int img_num2;//显示第img_num2张跳跃图片
int img_num3;//显示第img_num3张下滑图片
int move;//move为0代表跑，1代表跳，2代表滑
barrier barr;
int gameStatus; // 游戏状态，0为初始菜单界面，1为正常游戏，2为游戏规则，3为死亡游戏界面
int judge; //死亡判断 1为生，2为死,默认为生
int score; // 得分
int scoreNum;//判断背景循环次数，循环一次为3200
bool flag;//判断是否在跳跃的标志，FALSE表示目前未在跳跃过程中，TRUE表示正在跳跃过程中，这样做是为了避免长按w浮在空中的BUG

//函数声明
void startMenu();
void rules();
void showScore();
void startup();
void delay();
void restartup();
void runImg();
void jumpImg();
void slideImg();
void barrierImg();
void trapDeathImg();
void plantDeathImg();
void run();
void jump();
void slide();
void isBarrier();
void trapDeath();
void plantDeath();
void showdeathbk();
void die();
void show();
void updateWithoutInput();
void updateWithInput();
void gameover();

int main()//主函数
{
	PlaySound(TEXT("1.wav"), NULL, SND_LOOP | SND_ASYNC);  //设置背景音乐
	startup();
	while (1)
	{
		show();
		updateWithoutInput();
		updateWithInput();
	}
	gameover();
}

void startMenu()//开始界面显示
{
	putimage(0, 0, 800, 480, &img_gamestart_bg, 0, 0);//显示背景
	setbkmode(TRANSPARENT);//背景显示透明
	settextcolor(YELLOW);//字体颜色
	settextstyle(30, 0, _T("宋体"));//字体类型
	outtextxy(20, 400, _T("NEW GAME"));
	outtextxy(700, 400, _T("RULES"));
	settextcolor(BLACK);//字体颜色
	settextstyle(40, 0, _T("黑体"));//字体类型
	outtextxy(330, 400, _T("RUNNING"));
	rectangle(15, 400, 150, 430);//矩形边框
	rectangle(700, 400, 780, 430);//矩形边框
								  //显示游戏界面文字
	FlushBatchDraw();
	Sleep(2);
	MOUSEMSG m;		// 定义鼠标消息
	while (MouseHit())  //这个函数用于检测当前是否有鼠标消息
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x >= 20 && m.x <= 150 && m.y >= 410 && m.y <= 430)//鼠标点击“NEW GAME”开始游戏
		{
			gameStatus = 1;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x >= 700 && m.x <= 780 && m.y >= 410 && m.y <= 430)//鼠标点击“RULES”进入游戏规则界面
		{
			gameStatus = 2;
		}
	}

}

void rules()//游戏规则界面显示
{
	putimage(0, 0, 800, 480, &img_gamestart_bg, 0, 0);//显示背景
	setbkmode(TRANSPARENT);//背景透明
	settextcolor(BLACK);//字体颜色
	settextstyle(40, 0, _T("黑体"));//字体类型
	outtextxy(20, 400, _T("s键下滑 w键跳跃"));
	settextstyle(50, 0, _T("黑体"));
	settextcolor(BLUE);
	outtextxy(650, 410, _T("START"));
	rectangle(650, 410, 780, 460);
	//显示游戏界面文字

	FlushBatchDraw();
	Sleep(2);

	MOUSEMSG m;		// 定义鼠标消息
	while (MouseHit())  //这个函数用于检测当前是否有鼠标消息
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x >= 645 && m.x <= 760 && m.y >= 415 && m.y <= 455)//鼠标点击“START”开始游戏
		{
			gameStatus = 1;
		}
	}
}

void showScore()//显示得分情况
{
	settextcolor(RED);//字体颜色
	settextstyle(30, 0, _T("黑体"));//字体类型
	outtextxy(WIDTH*0.4, HIGH*0.9, _T("得分："));
	TCHAR s[500];
	swprintf_s(s, _T("%d"), score / 10);
	outtextxy(WIDTH*0.5, HIGH*0.9, s);
}

void startup()//数据初始化
{
	initgraph(WIDTH, HIGH);//显示画布
	BeginBatchDraw();
	loadimage(&img_bg, _T(".\\Running\\bg.jpg"));
	loadimage(&img_run, _T(".\\Running\\runlast.jpg"));
	loadimage(&img_run1, _T(".\\Running\\runlast_1.jpg"));
	loadimage(&img_jump, _T(".\\Running\\jumps2.jpg"));
	loadimage(&img_jump1, _T(".\\Running\\jumps2_1.jpg"));
	loadimage(&img_slide, _T(".\\Running\\slide.jpg"));
	loadimage(&img_slide1, _T(".\\Running\\slide_1.jpg"));
	loadimage(&img_trap, _T(".\\Running\\trap.jpg"));
	loadimage(&img_trap1, _T(".\\Running\\trap_1.jpg"));
	loadimage(&img_dowmPlant, _T(".\\Running\\plantGreen.jpg"));
	loadimage(&img_dowmPlant1, _T(".\\Running\\plantGreen_1.jpg"));
	loadimage(&img_upPlant, _T(".\\Running\\ivyAnim1.jpg"));
	loadimage(&img_upPlant1, _T(".\\Running\\ivyAnim1_1.jpg"));
	loadimage(&img_gamestart_bg, _T(".\\Running\\gameStart.jpg"));
	loadimage(&img_trapDeath, _T(".\\Running\\deathTrapRunInto.jpg"));
	loadimage(&img_trapDeath1, _T(".\\Running\\deathTrapRunInto_1.jpg"));
	loadimage(&img_plantDeath, _T(".\\Running\\plantDeath.jpg"));
	loadimage(&img_plantDeath1, _T(".\\Running\\plantDeath_1.jpg"));
	loadimage(&img_deathbk, _T(".\\Running\\death.jpg"));
	img_bg_i = 0;//初始化为0
	M_X = 300;
	M_Y = 290;//定义人物位置
	begin_i = 0;
	barr.begin_i = 800;
	img_num1 = 0;//初始化显 示第0张跑动图片
	img_num2 = 0;//初始化显示第0张跳跃图片
	img_num3 = 0;//初始化显示第0张下滑图片
	barr.img_num = 0;//初始化显示第0张障碍物图片
	score = 0;
	gameStatus = 0;
	scoreNum = 0;
	flag = FALSE;
	srand((unsigned)time(NULL));//产生随机障碍物的随机数种子
	BeginBatchDraw();
	while (gameStatus == 0)
		startMenu(); // 初始菜单界面
}

// 延时函数
void delay(unsigned int xms) // xms代表需要延时的毫秒数
{
	unsigned int x, y;
	for (x = xms; x > 0; x--)
		for (y = 1000; y > 0; y--);
}

void restartup()//游戏重新开始，数据重新初始化
{
	img_bg_i = 0;//初始化为0
	M_X = 300;
	M_Y = 290;//定义人物位置
	begin_i = 0;
	barr.begin_i = 800;
	img_num1 = 0;//初始化显 示第0张跑动图片
	img_num2 = 0;//初始化显示第0张跳跃图片
	img_num3 = 0;//初始化显示第0张下滑图片
	barr.img_num = 0;//初始化显示第0张障碍物图片
	score = 0;//得分为0
	gameStatus = 0;//初始在菜单界面
	scoreNum = 0;//背景循环次数初始为0
	move = 0;
	flag = FALSE;
}

void runImg(int x, int y)//跑动图片的显示
{
	putimage(M_X, M_Y, R_W, R_H, &img_run1, x, y, NOTSRCERASE);
	putimage(M_X, M_Y, R_W, R_H, &img_run, x, y, SRCINVERT);
}

void jumpImg(int w, int h, int i, int j)//跳跃图片的显示
{
	putimage(M_X, M_Y, w, h, &img_jump1, i, j, NOTSRCERASE);
	putimage(M_X, M_Y, w, h, &img_jump, i, j, SRCINVERT);
}

void slideImg(int X, int Y, int w, int h, int i, int j)//下滑图片的显示
{
	putimage(X, Y, w, h, &img_slide1, i, j, NOTSRCERASE);
	putimage(X, Y, w, h, &img_slide, i, j, SRCINVERT);
}

void barrierImg(int id, int x, int i, int j, int w, int h)//障碍物图片的显示
{
	if (id == 1)
	{
		putimage(x, 410 - h, w, h, &img_trap1, i, j, NOTSRCERASE);
		putimage(x, 410 - h, w, h, &img_trap, i, j, SRCINVERT);
	}
	else if (id == 2)
	{
		putimage(x, 410 - h, w, h, &img_dowmPlant1, i, j, NOTSRCERASE);
		putimage(x, 410 - h, w, h, &img_dowmPlant, i, j, SRCINVERT);
	}
	else if (id == 3)
	{
		putimage(x, 0, w, h, &img_upPlant1, i, j, NOTSRCERASE);
		putimage(x, 0, w, h, &img_upPlant, i, j, SRCINVERT);
	}
}

void trapDeathImg(int y, int i, int j, int w, int h)////碰到地刺死亡图片的显示
{
	putimage(M_X, y, w, h, &img_trapDeath1, i, j, NOTSRCERASE);
	putimage(M_X, y, w, h, &img_trapDeath, i, j, SRCINVERT);
}

void plantDeathImg(int i, int j, int w, int h)//碰到藤蔓死亡的图片显示
{
	putimage(M_X, 400 - h, w, h, &img_plantDeath1, i, j, NOTSRCERASE);
	putimage(M_X, 400 - h, w, h, &img_plantDeath, i, j, SRCINVERT);
}

void run(int i)//人物跑动
{
	end_i = i;
	pass_i = end_i - begin_i;
	if (pass_i == 8)
	{
		img_num1++;//显示下一张跑动图片
		begin_i = end_i;
	}
	if (img_num1 == 19)//当等于19时，显示第0张
		img_num1 = 0;

	switch (img_num1)
	{
	case 0:runImg(R_W, 0); break;
	case 1:runImg(0, R_H); break;
	case 2:runImg(R_W, R_H); break;
	case 3:runImg(2 * R_W, 0); break;
	case 4:runImg(3 * R_W, 0); break;
	case 5:runImg(4 * R_W, 0); break;
	case 6:runImg(3 * R_W, R_H); break;
	case 7:runImg(4 * R_W, R_H); break;
	case 8:runImg(0, 2 * R_H); break;
	case 9:runImg(2 * R_W, 2 * R_H); break;
	case 10:runImg(3 * R_W, 2 * R_H); break;
	case 11:runImg(4 * R_W, 2 * R_H); break;
	case 12:runImg(5 * R_W, 0); break;
	case 13:runImg(6 * R_W, 0); break;
	case 14:runImg(7 * R_W, 0); break;
	case 15:runImg(6 * R_W, R_H); break;
	case 16:runImg(5 * R_W, 2 * R_H); break;
	case 17:runImg(7 * R_W, R_H); break;
	case 18:runImg(6 * R_W, 2 * R_H); break;
	}
}

void jump(int i)//人物跳跃
{
	end_i = i;
	pass_i = end_i - begin_i;
	if (pass_i == 23)
	{
		img_num2++;//显示下一张跳跃图片
				   //计算显示跳跃图片的位置
		if (img_num2 < 8 && img_num2>0)
			M_Y -= 23;
		else if (img_num2 > 8 && img_num2 < 16)
			M_Y += 23;

		begin_i = end_i;
	}

	switch (img_num2)
	{
	case 0:jumpImg(90, 110, 195, 0); break;
	case 1:jumpImg(50, 115, 290, 0); break;
	case 2:jumpImg(55, 115, 195, 115); break;
	case 3:jumpImg(50, 115, 290, 115); break;
	case 4:jumpImg(50, 115, 380, 0); break;
	case 5:jumpImg(60, 115, 375, 115); break;
	case 6:jumpImg(70, 110, 10, 230); break;
	case 7:jumpImg(70, 110, 190, 230); break;
	case 8:jumpImg(75, 105, 275, 235); break;
	case 9:jumpImg(70, 110, 370, 230); break;
	case 10:jumpImg(60, 115, 465, 0); break;
	case 11:jumpImg(55, 115, 555, 0); break;
	case 12:jumpImg(60, 115, 465, 115); break;
	case 13:jumpImg(65, 115, 640, 0); break;
	case 14:jumpImg(75, 110, 545, 115); break;
	case 15:jumpImg(75, 100, 455, 235); break;
	}
}

void slide(int i)//人物下滑
{
	end_i = i;
	pass_i = end_i - begin_i;
	if (pass_i == 20)
	{
		img_num3++;//显示下一张下滑图片
		begin_i = end_i;
	}

	switch (img_num3)
	{

	case 0:slideImg(M_X, 283, 70, 110, 550, 112); break;
	case 1:slideImg(M_X, 285, 80, 110, 540, 0); break;
	case 2:slideImg(M_X, 300, 90, 100, 405, 238); break;
	case 3:slideImg(M_X, 310, 120, 100, 258, 250); break;
	case 4:slideImg(M_X, 310, 110, 90, 382, 27); break;
	case 5:slideImg(M_X, 310, 110, 90, 382, 27); break;
	case 6:slideImg(M_X, 310, 110, 90, 382, 27); break;
	case 7:slideImg(M_X, 300, 100, 100, 636, 239); break;
	case 8:slideImg(M_X, 300, 95, 110, 510, 350); break;
	case 9:slideImg(M_X, 300, 95, 110, 627, 122); break;
	case 10:slideImg(M_X, 300, 100, 100, 500, 225); break;
	case 11:slideImg(M_X, 270, 80, 110, 125, 330); break;

	}
}

void isBarrier(int i)//障碍物显示
{
	int imgCount;//不同障碍物图片的数量

	if (i % 800 == 0)
	{
		barr.id = rand() % 3 + 1;//产生随机种类的障碍物
		barr.img_num = 0;
	}

	if (barr.id == 1)
		imgCount = 14;
	else if (barr.id == 2)
		imgCount = 6;
	else if (barr.id == 3)
		imgCount = 6;

	barr.end_i = i;
	barr.pass_i = barr.end_i - barr.begin_i;
	if (barr.pass_i == -20)
	{
		barr.img_num++;//显示下一张图片
		barr.begin_i = barr.end_i;
	}
	if (barr.img_num == imgCount)//循环显示
		barr.img_num = 0;

	if (barr.id == 1)
	{
		switch (barr.img_num)
		{
		case 0:barrierImg(barr.id, i, 30, 55, 140, 25); break;
		case 1:barrierImg(barr.id, i, 30, 135, 140, 35); break;
		case 2:barrierImg(barr.id, i, 30, 215, 140, 40); break;
		case 3:barrierImg(barr.id, i, 245, 40, 140, 45); break;
		case 4:barrierImg(barr.id, i, 245, 120, 140, 50); break;
		case 5:barrierImg(barr.id, i, 245, 200, 140, 55); break;
		case 6:barrierImg(barr.id, i, 30, 280, 140, 60); break;
		case 7:barrierImg(barr.id, i, 30, 340, 140, 85); break;
		case 8:barrierImg(barr.id, i, 30, 280, 140, 60); break;
		case 9:barrierImg(barr.id, i, 245, 200, 140, 55); break;
		case 10:barrierImg(barr.id, i, 245, 120, 140, 50); break;
		case 11:barrierImg(barr.id, i, 245, 40, 140, 45); break;
		case 12:barrierImg(barr.id, i, 30, 215, 140, 40); break;
		case 13:barrierImg(barr.id, i, 30, 135, 140, 35); break;
		}
	}
	else if (barr.id == 2)
	{
		switch (barr.img_num)
		{
		case 0:barrierImg(barr.id, i, 0, 0, 165, 130); break;
		case 1:barrierImg(barr.id, i, 165, 0, 165, 130); break;
		case 2:barrierImg(barr.id, i, 2 * 165, 0, 165, 130); break;
		case 3:barrierImg(barr.id, i, 3 * 165, 0, 165, 130); break;
		case 4:barrierImg(barr.id, i, 4 * 165, 0, 165, 130); break;
		case 5:barrierImg(barr.id, i, 5 * 165, 0, 165, 130); break;
		}
	}
	else
	{
		switch (barr.img_num)
		{
		case 0:barrierImg(barr.id, i, 0, 25, 265, 310); break;
		case 1:barrierImg(barr.id, i, 265, 25, 265, 310); break;
		case 2:barrierImg(barr.id, i, 2 * 265, 25, 265, 310); break;
		case 3:barrierImg(barr.id, i, 0, 372, 265, 315); break;
		case 4:barrierImg(barr.id, i, 265, 372, 265, 315); break;
		case 5:barrierImg(barr.id, i, 2 * 265, 372, 265, 315); break;
		}
	}
}

void trapDeath()//碰到地刺死亡
{
	int num = 0;//显示第num张图片
	while (num < 15)
	{
		//背景障碍覆盖，为了图片能逐张显示
		if (img_bg_i < 3 * WIDTH)
			putimage(0, 0, WIDTH, HIGH, &img_bg, img_bg_i, 0);
		else if (img_bg_i >= 3 * WIDTH&&img_bg_i < 4 * WIDTH)//背景循环
		{
			putimage(0, 0, 4 * WIDTH - img_bg_i, HIGH, &img_bg, img_bg_i, 0);
			putimage(4 * WIDTH - img_bg_i, 0, img_bg_i - 3 * WIDTH, HIGH, &img_bg, 0, 0);
		}
		isBarrier(barr.i);

		switch (num)
		{
		case 0:trapDeathImg(290, 0, 0, 95, 100); break;
		case 1:trapDeathImg(290, 380, 116, 100, 100); break;
		case 2:trapDeathImg(294, 10, 240, 100, 96); break;
		case 3:trapDeathImg(297, 10, 361, 100, 93); break;
		case 4:trapDeathImg(314, 133, 250, 100, 86); break;
		case 5:trapDeathImg(317, 123, 382, 98, 81); break;
		case 6:trapDeathImg(326, 247, 273, 122, 79); break;
		case 7:trapDeathImg(324, 249, 390, 118, 81); break;
		case 8:trapDeathImg(325, 375, 272, 114, 80); break;
		case 9:trapDeathImg(325, 131, 37, 112, 80); break;
		case 10:trapDeathImg(325, 8, 154, 113, 80); break;
		case 11:trapDeathImg(325, 131, 154, 113, 80); break;
		case 12:trapDeathImg(325, 254, 36, 114, 80); break;
		case 13:trapDeathImg(325, 253, 154, 114, 80); break;
		case 14:trapDeathImg(325, 376, 36, 114, 80); break;
		}
		FlushBatchDraw();
		num++;
		Sleep(150);
	}
}

void plantDeath()//碰到藤曼死亡
{
	int num = 0;//显示第num张图片
	while (num < 23)
	{
		//背景障碍覆盖，为了图片能逐张显示
		if (img_bg_i < 3 * WIDTH)
			putimage(0, 0, WIDTH, HIGH, &img_bg, img_bg_i, 0);
		else if (img_bg_i >= 3 * WIDTH&&img_bg_i < 4 * WIDTH)//背景循环
		{
			putimage(0, 0, 4 * WIDTH - img_bg_i, HIGH, &img_bg, img_bg_i, 0);
			putimage(4 * WIDTH - img_bg_i, 0, img_bg_i - 3 * WIDTH, HIGH, &img_bg, 0, 0);
		}
		isBarrier(barr.i);

		switch (num)
		{
		case 0:plantDeathImg(3, 38, 57, 109); break;
		case 1:plantDeathImg(717, 36, 50, 111); break;
		case 2:plantDeathImg(363, 652, 40, 112); break;
		case 3:plantDeathImg(354, 805, 43, 112); break;
		case 4:plantDeathImg(205, 33, 41, 114); break;
		case 5:plantDeathImg(29, 188, 44, 113); break;
		case 6:plantDeathImg(28, 346, 51, 109); break;
		case 7:plantDeathImg(205, 192, 52, 109); break;
		case 8:plantDeathImg(205, 345, 51, 110); break;
		case 9:plantDeathImg(382, 345, 51, 110); break;
		case 10:plantDeathImg(382, 37, 51, 110); break;
		case 11:plantDeathImg(560, 193, 50, 108); break;
		case 12:plantDeathImg(560, 346, 48, 109); break;
		case 13:plantDeathImg(738, 191, 50, 110); break;
		case 14:plantDeathImg(740, 350, 45, 105); break;
		case 15:plantDeathImg(40, 516, 60, 98); break;
		case 16:plantDeathImg(40, 675, 87, 89); break;
		case 17:plantDeathImg(216, 530, 112, 84); break;
		case 18:plantDeathImg(40, 850, 123, 67); break;
		case 19:plantDeathImg(218, 702, 116, 62); break;
		case 20:plantDeathImg(394, 563, 134, 51); break;
		case 21:plantDeathImg(216, 876, 134, 41); break;
		case 22:plantDeathImg(570, 580, 137, 44); break;
		}
		FlushBatchDraw();
		num++;
		Sleep(150);
	}
}

void showdeathbk()//显示死亡游戏背景界面
{
	Sleep(150);
	putimage(0, 0, &img_deathbk);//游戏背景界面
	setbkmode(TRANSPARENT);//背景显示透明
	settextcolor(YELLOW);//字体颜色
	settextstyle(30, 0, _T("宋体"));//字体类型
	outtextxy(330, 70, _T("NEW GAME"));
	outtextxy(360, 190, _T("MENU"));
	outtextxy(360, 305, _T("EXIT"));
	settextcolor(BLACK);//字体颜色
	settextstyle(50, 0, _T("宋体"));//字体类型
	outtextxy(30, 420, _T("GAME OVER"));
	showScore();//显示得分情况
				//显示游戏界面文字
	FlushBatchDraw();
	Sleep(2);
	MOUSEMSG m;		// 定义鼠标消息
	while (MouseHit())  //这个函数用于检测当前是否有鼠标消息
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x >= 330 && m.x <= 450 && m.y >= 70 && m.y <= 100)//鼠标点击“NEW GAME”重新开始游戏
		{
			restartup();//数据初始化
			gameStatus = 1;//游戏状态变成1
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x >= 330 && m.x <= 450 && m.y >= 190 && m.y <= 220)//鼠标点击“MENU”进入游戏规则界面
		{
			restartup();//数据初始化
			gameStatus = 0;//游戏状态变成0，菜单界面
			while (gameStatus == 0)
			{
				startMenu();
			}

		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x >= 330 && m.x <= 450 && m.y >= 300 && m.y <= 330)//鼠标点击“EXIT”退出雨欣
		{
			restartup();//初始化数据
			exit(0);//退出游戏
			closegraph();
		}
	}
	FlushBatchDraw();

}

void die()//判断死亡
{
	//如果遇到的障碍物是地刺，在地刺区域内跑和滑都会死亡
	if (barr.id == 1)
	{
		if (move == 0 || move == 2)
			if (M_X + 40 >= barr.i&&M_X <= barr.i + 95)
			{
				flag = FALSE;
				trapDeath();
				gameStatus = 3;
			}
	}
	//如果遇到的障碍物是下藤曼，在藤曼区域内跑和滑都会死亡
	else if (barr.id == 2)
	{
		if (move == 0 || move == 2)
			if (M_X + 50 >= barr.i&&M_X <= barr.i + 157)
			{
				flag = FALSE;
				plantDeath();
				gameStatus = 3;
			}
	}
	//如果遇到的障碍物是上藤曼，在藤曼区域内跑和跳都会死亡
	else
	{
		if (move == 0 || move == 1)
			if (M_X + 40 >= barr.i&&M_X <= barr.i + 265)
			{
				plantDeath();
				gameStatus = 3;
			}
	}
}

void show()//显示
{
	while (gameStatus == 0)//游戏状态为0，显示菜单界面
	{
		startMenu();
	}
	while (gameStatus == 2)//游戏状态为2，显示规则界面
	{
		rules();
	}
	while (gameStatus == 3)//游戏状态为3，显示死亡背景界面
	{
		showdeathbk();
	}

	//背景播放
	if (img_bg_i < 3 * WIDTH)
	{
		putimage(0, 0, WIDTH, HIGH, &img_bg, img_bg_i, 0);

		if (img_bg_i < WIDTH)//获得一个0-800的位置
			barr.i = WIDTH - img_bg_i;
		else if (img_bg_i >= WIDTH && img_bg_i < 2 * WIDTH)
			barr.i = 2 * WIDTH - img_bg_i;
		else if (img_bg_i >= 2 * WIDTH&&img_bg_i < 3 * WIDTH)
			barr.i = 3 * WIDTH - img_bg_i;

		isBarrier(barr.i);
		showScore();
		//判断执行动作
		if (move == 0)
			run(img_bg_i);
		else if (move == 1)
		{
			jump(img_bg_i);
			if (img_num2 == 15)
			{
				flag = FALSE;
				move = 0;//一次完整的跳跃后跑
				img_num2 = 0;
				img_num1 = 0;//跳完后开始跑
			}
		}
		else if (move == 2)
		{
			slide(img_bg_i);
			if (img_num3 == 11)
			{
				move = 0;//一次完整的下滑后跑
				img_num3 = 0;
				img_num1 = 0;//滑完后开始跑
			}
		}
		FlushBatchDraw();
	}
	else if (img_bg_i >= 3 * WIDTH&&img_bg_i < 4 * WIDTH)//背景循环
	{
		putimage(0, 0, 4 * WIDTH - img_bg_i, HIGH, &img_bg, img_bg_i, 0);
		putimage(4 * WIDTH - img_bg_i, 0, img_bg_i - 3 * WIDTH, HIGH, &img_bg, 0, 0);

		barr.i = 4 * WIDTH - img_bg_i;//获得一个0-800的位置
		isBarrier(barr.i);
		showScore();

		if (move == 0)
			run(img_bg_i);
		else if (move == 1)
		{
			jump(img_bg_i);
			if (img_num2 == 15)
			{
				move = 0;
				img_num2 = 0;
				img_num1 = 0;
			}
		}
		else if (move == 2)
		{
			slide(img_bg_i);
			if (img_num3 == 12)
			{
				move = 0;//一次完整的下滑后跑
				img_num3 = 0;
				img_num1 = 0;//滑完后开始跑
			}
		}
		FlushBatchDraw();
	}

	img_bg_i++;//分割线右移，图片移动
	if (img_bg_i == 4 * WIDTH)
	{
		img_bg_i = 0;
		begin_i = 0;
		scoreNum++;
	}

	if (img_bg_i%WIDTH == 0)
		barr.begin_i = 800;
	delay(T);
}

void updateWithoutInput()//与用户无关的输入
{
	die();
	if (img_bg_i < 3200)//背景循环一次，像素经过3200
		score = scoreNum * 3200 + img_bg_i;
}

void updateWithInput()//与用户有关的输入
{
	char ch;
	if (_kbhit())
	{
		ch =_getch();
		if (ch == 'w'&&flag==FALSE)
		{
			move = 1;
			begin_i = img_bg_i;//获取按键时的img_bg_i
			flag = TRUE;
		}

		if (ch == 's')
		{
			move = 2;
			begin_i = img_bg_i;//获取按键时的img_bg_i
			flag = FALSE;
		}
	}
}

void gameover()//游戏结束
{
	EndBatchDraw();
	_getch();
	closegraph();
}