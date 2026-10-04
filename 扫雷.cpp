#include <stdio.h>
#include <graphics.h>
#include <stdlib.h>
#include <time.h>

#define UNIT_SIZE 50
#define MAP_SIZE 10
#define TNT_NUM 10
#define IMG_STATE 14

/*
可以优化的地方：
1.打印单元格那里，遇到0值时，采用递归方法实现，当地图尺寸较大时，会占用栈内存，严重时导致栈溢出，需优化
2.代码有些重复的地方，可以包装为函数，比如遍历自身8个近邻格（这个可能包装不了，尝试一下）。
3.变量的定义和使用方式可以优化，集中到一块区域内。
4.Initimg函数使用了简单粗暴的复制粘贴，可以想办法简化
5.这是C++，单元格是否打印数组可以用bool表示，毕竟只有0/1
*/

void creatTNT(int(*Map)[MAP_SIZE])
{
	srand(time(NULL));
	for (int i = 0; i < TNT_NUM; )
	{
		int x = rand() % MAP_SIZE;
		int y = rand() % MAP_SIZE;
		if (Map[x][y] == 0)
		{
			Map[x][y] = -1;
			i++;
		}
	}
}

int getTNTnum(int(* Map)[MAP_SIZE], int x, int y)
{
	int num = 0;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (i == 0 && j == 0)
				continue;
			if ((x + i) < 0 || (x + i) >= MAP_SIZE || (y + j) < 0 || (y + j) >= MAP_SIZE)
				continue;
			if (Map[x + i][y + j] == -1)
			{
				num++;
			}
		}
	}
	return num;
}

void InitMap(int(*Map)[MAP_SIZE])
{
	creatTNT(Map);
	for (int i = 0; i < MAP_SIZE; i++)
	{
		for (int j = 0; j < MAP_SIZE; j++)
		{
			if (Map[i][j] != -1)
			{
				Map[i][j] = getTNTnum(Map, i, j);
			}
		}
	}
}

void InitImg(IMAGE* imgArr)
{
	loadimage(&imgArr[0], L"image\\0.jpg");
	loadimage(&imgArr[1], L"image\\1.jpg");
	loadimage(&imgArr[2], L"image\\2.jpg");
	loadimage(&imgArr[3], L"image\\3.jpg");
	loadimage(&imgArr[4], L"image\\4.jpg");
	loadimage(&imgArr[5], L"image\\5.jpg");
	loadimage(&imgArr[6], L"image\\6.jpg");
	loadimage(&imgArr[7], L"image\\7.jpg");
	loadimage(&imgArr[8], L"image\\8.jpg");
	loadimage(&imgArr[9], L"image\\9.jpg");
	loadimage(&imgArr[10], L"image\\10.jpg");
	loadimage(&imgArr[11], L"image\\11.jpg");
	loadimage(&imgArr[12], L"image\\12.jpg");
	loadimage(&imgArr[13], L"image\\13.jpg");

}

void printAnswer(int(*Map)[MAP_SIZE], IMAGE* imgArr)
{
	for (int i = 0; i < MAP_SIZE; i++)
	{
		for (int j = 0; j < MAP_SIZE; j++)
		{
			if (Map[i][j] == -1)
			{
				putimage(i * UNIT_SIZE, j * UNIT_SIZE, &imgArr[12]);
			}
			else
			{
				putimage(i * UNIT_SIZE, j * UNIT_SIZE, &imgArr[Map[i][j]]);
			}
		}
	}
}

void printUnit(int(*Map)[MAP_SIZE],int x,int y, IMAGE* imgArr, int(*UnitShowState)[MAP_SIZE],int *Num)
{
	if (UnitShowState[x][y] == 1)
		return;
	putimage(x * UNIT_SIZE, y * UNIT_SIZE, &imgArr[Map[x][y]]);
	(*Num)++;
	UnitShowState[x][y] = 1;
	if (Map[x][y] == 0)
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (i == 0 && j == 0)
					continue;
				if ((x + i) < 0 || (x + i) >= MAP_SIZE || (y + j) < 0 || (y + j) >= MAP_SIZE)
					continue;
				printUnit(Map, (x + i), (y + j), imgArr, UnitShowState,Num);
			}
		}
	}
	return;
}

void printBackground( IMAGE* imgArr)
{
	for (int i = 0; i < MAP_SIZE; i++)
	{
		for (int j = 0; j < MAP_SIZE; j++)
		{
			putimage(i * UNIT_SIZE, j * UNIT_SIZE, &imgArr[9]);
		}
	}
}

void CheckMouseClick(int* Click,int* x,int* y)
{//格式：[state,x,y]  鼠标左键1，右键0
	MOUSEMSG msg;
	while (1)
	{
		msg = GetMouseMsg();
		if (msg.uMsg == WM_LBUTTONDOWN || msg.uMsg == WM_RBUTTONDOWN)
		{
			*Click = (msg.uMsg == WM_LBUTTONDOWN) ? 1 : 0;
			*x = msg.x / UNIT_SIZE;
			*y = msg.y / UNIT_SIZE; 
			if (*x >= 0 && *x < MAP_SIZE && *y >= 0 && *y < MAP_SIZE)
			{
				return;
			}
		}
	}
}
int PlayGame(int(*Map)[MAP_SIZE], IMAGE* imgArr)
{
	int UnitShowNum = 0;
	int UnitFlogState[MAP_SIZE][MAP_SIZE] = { 0 };//0无1旗2未知
	int UnitShowState[MAP_SIZE][MAP_SIZE] = { 0 };
	int MouseClick, MouseX, MouseY;
	printBackground(imgArr);
	while (1)
	{
		CheckMouseClick(&MouseClick, &MouseX, &MouseY);
		if (MouseClick == 1)//鼠标左键
		{
			if (Map[MouseX][MouseY] == -1)//点击到炸弹
			{
				printAnswer(Map, imgArr);
				putimage(MouseX * UNIT_SIZE, MouseY * UNIT_SIZE, &imgArr[13]);
				return 0;
			}
			else //点击到非炸弹
			{
				printUnit(Map, MouseX, MouseY, imgArr, UnitShowState,&UnitShowNum);
			}
		}
		else //鼠标右键
		{
			UnitFlogState[MouseX][MouseY]++;
			if (UnitFlogState[MouseX][MouseY] == 3)
				UnitFlogState[MouseX][MouseY] -= 3;
			putimage(MouseX * UNIT_SIZE, MouseY * UNIT_SIZE, &imgArr[UnitFlogState[MouseX][MouseY]+9]);
		}
		if (UnitShowNum >= MAP_SIZE * MAP_SIZE - TNT_NUM)
		{
			printAnswer(Map, imgArr);
			return 1;
		}
	}
}

int main()
{
	int Map[MAP_SIZE][MAP_SIZE] = { 0 };
	IMAGE imgArr[IMG_STATE];

	initgraph(UNIT_SIZE * MAP_SIZE, UNIT_SIZE * MAP_SIZE);
	InitMap(Map);
	InitImg(imgArr);

	
	if (PlayGame(Map, imgArr) == 1)
	{
		printf("恭喜你，胜利了！\n按任意键结束游戏\n");
	}
	else
	{
		printf("你失败了，再接再厉。\n按任意键结束游戏\n");
	}
	char c = getchar();

	return 0;
}