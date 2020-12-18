/*
 *        Computer Graphics Course - Shenzhen University
 *    Mid-term Assignment - Tetris implementation sample code
 * ============================================================
 *
 * - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
 * - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
 *
 * - 已实现功能如下：
 * - 1) 绘制棋盘格和‘L’型方块
 * - 2) 键盘左/右/下键控制方块的移动，上键旋转方块
 *
 * - 未实现功能如下：
 * - 1) 随机生成方块并赋上不同的颜色
 * - 2) 方块的自动向下移动
 * - 3) 方块之间的碰撞检测
 * - 4) 棋盘格中每一行填充满之后自动消除
 * - 5) 其他
 *
 */

#include "include/Angel.h"

#pragma comment(lib, "glew32.lib")

#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

int starttime;			// 控制方块向下移动时间
int rotation = 0;		// 控制当前窗口中的方块旋转
vec2 tile[4];			// 表示当前窗口中的方块
bool gameover = false;	// 游戏结束控制变量
int xsize = 400;		// 窗口大小（尽量不要变动窗口大小！）
int ysize = 720;

int Nindex;//设置一个序号确定形状
int staytime=1000;//默认方块下落速度为1000ms，即1s

vec2 ttile[4];//表示下一个可能的方块，若可能性成立，只需赋值给tile

// 一个二维数组表示所有可能出现的方块和方向。
vec2 allRotationsLshape[4][4] =
							  {{vec2(0, 0), vec2(-1,0), vec2(1,0), vec2(-1,-1)},	//   "L"
							   {vec2(0, 1), vec2(0, 0), vec2(0,-1), vec2(1, -1)},   //
							   {vec2(1, 1), vec2(-1,0), vec2(0, 0), vec2(1,  0)},   //
							   {vec2(-1,1), vec2(0, 1), vec2(0, 0), vec2(0, -1)}};
vec2 allRotationsOshape[4][4] = {{vec2(0, 0), vec2(-1,0), vec2(0,-1), vec2(-1,-1)},	//   "O",即正方形
                               { vec2(0, 0), vec2(-1,0), vec2(0,-1), vec2(-1,-1) },   //
							   { vec2(0, 0), vec2(-1,0), vec2(0,-1), vec2(-1,-1) },   //
							   { vec2(0, 0), vec2(-1,0), vec2(0,-1), vec2(-1,-1) }};
vec2 allRotationsIshape[4][4] = {{vec2(0, 0), vec2(-1,0), vec2(-2,0), vec2(1,0)},	//   "I",即直线
                               {vec2(0, 0), vec2(0,1), vec2(0,-1), vec2(0,-2) },   //
							   { vec2(0, 0), vec2(-1,0), vec2(-2,0), vec2(1,0) },   //
							   { vec2(0, 0), vec2(0,1), vec2(0,-1), vec2(0,-2) }};
vec2 allRotationsSshape[4][4] = {{vec2(0, 0), vec2(0,-1), vec2(-1,-1), vec2(1,0)},	//   "S"
                               {vec2(0, 0), vec2(0,1), vec2(1,0), vec2(1,-1) },   //
							   { vec2(0, 0), vec2(0,-1), vec2(-1,-1), vec2(1,0) },   //
							   { vec2(0, 0), vec2(0,1), vec2(1,0), vec2(1,-1) }};
vec2 allRotationsZshape[4][4] = {{vec2(0, 0), vec2(0,-1), vec2(1,-1), vec2(-1,0)},	//   "Z"
                               {vec2(0, 0), vec2(0,-1), vec2(1,0), vec2(1,1) },   //
							   { vec2(0, 0), vec2(0,-1), vec2(1,-1), vec2(-1,0) },   //
							   { vec2(0, 0), vec2(0,-1), vec2(1,0), vec2(1,1) }};
vec2 allRotationsJshape[4][4] ={{vec2(0, 0), vec2(-1,0), vec2(1,0), vec2(1,-1)},	//   "J"
							   {vec2(0, 1), vec2(0, 0), vec2(0,-1), vec2(1, 1)},   //
							   {vec2(-1, 1), vec2(-1,0), vec2(0, 0), vec2(1,  0)},   //
							   {vec2(-1,-1), vec2(0, 1), vec2(0, 0), vec2(0, -1)}};
vec2 allRotationsTshape[4][4] = {{vec2(0, 0), vec2(0,-1), vec2(-1,0), vec2(1,0)},	//   "T"
                               {vec2(0, 0), vec2(0,1), vec2(0,-1), vec2(1,0) },   //
							   { vec2(0, 0), vec2(-1,0), vec2(0,1), vec2(1,0) },   //
							   { vec2(0, 0), vec2(0,1), vec2(0,-1), vec2(-1,0) }};



// 绘制窗口的颜色变量
vec4 orange = vec4(1.0, 0.5, 0.0, 1.0);
vec4 white  = vec4(1.0, 1.0, 1.0, 1.0);
vec4 black  = vec4(0.0, 0.0, 0.0, 1.0);

vec4 yellow = vec4(1.0, 0.99, 0.6, 1.0);
vec4 blue = vec4(0.0, 0.65, 0.69, 1.0);
vec4 green = vec4(0.11, 0.75, 0.59, 1.0);
vec4 pink = vec4(0.98, 0.62, 0.6, 1.0);
vec4 purple = vec4(0.47, 0.2, 0.38, 1.0);
vec4 red = vec4(1.0, 0.26, 0.4, 1.0);
vec4 gray = vec4(0.71, 0.76, 0.6, 1.0);




// 当前方块的位置（以棋盘格的左下角为原点的坐标系）
vec2 tilepos = vec2(5, 19);

// 布尔数组表示棋盘格的某位置是否被方块填充，即board[x][y] = true表示(x,y)处格子被填充。
// （以棋盘格的左下角为原点的坐标系）
bool board[10][20];



// 当棋盘格某些位置被方块填充之后，记录这些位置上被填充的颜色
vec4 boardcolours[1200];

GLuint locxsize;
GLuint locysize;

GLuint vaoIDs[3];
GLuint vboIDs[6];

//////////////////////////////////////////////////////////////////////////
// 修改棋盘格在pos位置的颜色为colour，并且更新对应的VBO

void changecellcolour(vec2 pos, vec4 colour)
{
	// 每个格子是个正方形，包含两个三角形，总共6个定点，并在特定的位置赋上适当的颜色
	for (int i = 0; i < 6; i++)
		boardcolours[(int)(6*(10*pos.y + pos.x) + i)] = colour;

	vec4 newcolours[6] = {colour, colour, colour, colour, colour, colour};

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);

	// 计算偏移量，在适当的位置赋上颜色
	int offset = 6 * sizeof(vec4) * (int)(10*pos.y + pos.x);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


//////////////////////////////////////////////////////////////////////////
// 当前方块移动或者旋转时，更新VBO

void updatetile()
{
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);

	// 每个方块包含四个格子
	for (int i = 0; i < 4; i++)
	{
		// 计算格子的坐标值
		GLfloat x = tilepos.x + tile[i].x;
		GLfloat y = tilepos.y + tile[i].y;

		vec4 p1 = vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
		vec4 p2 = vec4(33.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);
		vec4 p3 = vec4(66.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
		vec4 p4 = vec4(66.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);

		// 每个格子包含两个三角形，所以有6个顶点坐标
		vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4};
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), newpoints);
	}
	glBindVertexArray(0);
}
//设置下一个即将出现的方块的随机性
void createShape()
{
	int nindex = rand() % 7;//一共有7种形状

	Nindex = nindex;

	rotation = rand() % 4;//0~3   每种形状的方块有4个方向
	switch (nindex)
	{
	case 0:
		for (int i = 0; i < 4; i++)
		{
			tile[i] = allRotationsLshape[rotation][i];
		}
		break;
	case 1:
		for (int i = 0; i < 4; i++)
		{
			tile[i] = allRotationsOshape[rotation][i];
		}
		break;
	case 2:
		for (int i = 0; i < 4; i++)
		{
			tile[i] = allRotationsIshape[rotation][i];
		}
		break;
	case 3:
		for (int i = 0; i < 4; i++)
		{
			tile[i] = allRotationsSshape[rotation][i];
		}
		break;
	case 4:
		for (int i = 0; i < 4; i++)
		{
			tile[i] = allRotationsZshape[rotation][i];
		}
		break;
	case 5:
		for (int i = 0; i < 4; i++)
		{
			tile[i] = allRotationsJshape[rotation][i];
		}
		break;
	case 6:
		for (int i = 0; i < 4; i++)
		{
			tile[i] = allRotationsTshape[rotation][i];
		}
		break;
	}
}
//选择一个形状
void selectShape(int rotation)
{
	
	switch (Nindex)
	{
	case 0:
		for (int i = 0; i < 4; i++)
		{
			ttile[i] = allRotationsLshape[rotation][i];
		}
		break;
	case 1:
		for (int i = 0; i < 4; i++)
		{
			ttile[i] = allRotationsOshape[rotation][i];
		}
		break;
	case 2:
		for (int i = 0; i < 4; i++)
		{
			ttile[i] = allRotationsIshape[rotation][i];
		}
		break;
	case 3:
		for (int i = 0; i < 4; i++)
		{
			ttile[i] = allRotationsSshape[rotation][i];
		}
		break;
	case 4:
		for (int i = 0; i < 4; i++)
		{
			ttile[i] = allRotationsZshape[rotation][i];
		}
		break;
	case 5:
		for (int i = 0; i < 4; i++)
		{
			ttile[i] = allRotationsJshape[rotation][i];
		}
		break;
	case 6:
		for (int i = 0; i < 4; i++)
		{
			ttile[i] = allRotationsTshape[rotation][i];
		}
		break;
	}
}
//为每种形状指定一种颜色
vec4 selectColour()
{
	switch (Nindex)
	{
		case 0:
			return orange;
			break;
		case 1:
			return green;
			break;
		case 2:
			return blue;
			break;
		case 3:
			return yellow;
			break;
		case 4:
			return pink;
			break;
		case 5:
			return purple;
			break;
		case 6:
			return red;
			break;

	}
}
//////////////////////////////////////////////////////////////////////////
// 设置当前方块为下一个即将出现的方块。在游戏开始的时候调用来创建一个初始的方块，
// 在游戏结束的时候判断，没有足够的空间来生成新的方块。

void newtile()
{
	// 将新方块放于棋盘格的最上行中间位置并设置默认的旋转方向
	tilepos = vec2(5 , 19);
	rotation = 0;

	srand(time(NULL));

	if (board[5][18] == true)//设置结束条件   error！！！：弹出对话框后，若关闭对话框程序不能退出，会继续执行并再次弹出对话框。
	{
		MessageBox(NULL, "游戏结束！\n请选择‘r’（重新开始）或‘q’（退出游戏）。", "提示", MB_OK);

	}
	//用函数createShape设置下一个即将出现的方块的随机性
	/*for (int i = 0; i < 4; i++)
	{
		tile[i] = allRotationsLshape[0][i];
	}*/
	createShape();//随机创建一个形状任意方向的方块

	updatetile();//更新界面

	// 给新方块赋上颜色
	vec4 newcolours[24];
	for (int i = 0; i < 24; i++)
		//	newcolours[i] = orange;
		newcolours[i] = selectColour();//按照方块的形状选择相应的颜色

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////
// 游戏和OpenGL初始化

void init()
{
	// 初始化棋盘格，包含64个顶点坐标（总共32条线），并且每个顶点一个颜色值
	vec4 gridpoints[64];
	vec4 gridcolours[64];

	// 纵向线
	for (int i = 0; i < 11; i++)
	{
		gridpoints[2*i] = vec4((33.0 + (33.0 * i)), 33.0, 0, 1);
		gridpoints[2*i + 1] = vec4((33.0 + (33.0 * i)), 693.0, 0, 1);

	}

	// 水平线
	for (int i = 0; i < 21; i++)
	{
		gridpoints[22 + 2*i] = vec4(33.0, (33.0 + (33.0 * i)), 0, 1);
		gridpoints[22 + 2*i + 1] = vec4(363.0, (33.0 + (33.0 * i)), 0, 1);
	}

	// 将所有线赋成白色
	for (int i = 0; i < 64; i++)
		gridcolours[i] = white;

	// 初始化棋盘格，并将没有被填充的格子设置成黑色
	vec4 boardpoints[1200];
	for (int i = 0; i < 1200; i++)
		boardcolours[i] = black;

	// 对每个格子，初始化6个顶点，表示两个三角形，绘制一个正方形格子
	for (int i = 0; i < 20; i++)
		for (int j = 0; j < 10; j++)
		{
			vec4 p1 = vec4(33.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p2 = vec4(33.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			vec4 p3 = vec4(66.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p4 = vec4(66.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);

			boardpoints[6*(10*i + j)    ] = p1;
			boardpoints[6*(10*i + j) + 1] = p2;
			boardpoints[6*(10*i + j) + 2] = p3;
			boardpoints[6*(10*i + j) + 3] = p2;
			boardpoints[6*(10*i + j) + 4] = p3;
			boardpoints[6*(10*i + j) + 5] = p4;
		}

	// 将棋盘格所有位置的填充与否都设置为false（没有被填充）
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
			board[i][j] = false;

	// 载入着色器
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	locxsize = glGetUniformLocation(program, "xsize");
	locysize = glGetUniformLocation(program, "ysize");

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	GLuint vColor = glGetAttribLocation(program, "vColor");

	glGenVertexArrays(3, &vaoIDs[0]);

	// 棋盘格顶点
	glBindVertexArray(vaoIDs[0]);
	glGenBuffers(2, vboIDs);

	// 棋盘格顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridcolours, GL_STATIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	// 棋盘格每个格子
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// 棋盘格每个格子顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格每个格子顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	// 当前方块
	glBindVertexArray(vaoIDs[2]);
	glGenBuffers(2, &vboIDs[4]);

	// 当前方块顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 当前方块顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);

	// 游戏初始化
	newtile();
	starttime = glutGet(GLUT_ELAPSED_TIME);
}

//////////////////////////////////////////////////////////////////////////
// 检查在cellpos位置的格子是否被填充或者是否在棋盘格的边界范围内。

bool checkvalid(vec2 cellpos)
{
	if((cellpos.x >=0) && (cellpos.x < 10) && (cellpos.y >= 0) && (cellpos.y < 20) )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
// 在棋盘上有足够空间的情况下旋转当前方块

void rotate()
{
	// 计算得到下一个旋转方向
	int nextrotation = (rotation + 1) % 4;
	int tempx[4], tempy[4];
	selectShape(nextrotation);//选择一个形状
	for (int i = 0; i < 4; i++)
	{//假设旋转后格子的坐标
		tempx[i] = (ttile[i] + tilepos).x;// 获取格子在棋盘格上的横坐标
		tempy[i] = (ttile[i] + tilepos).y;// 获取格子在棋盘格上的纵坐标
	}
	// 检查当前旋转之后的位置的有效性
	if (checkvalid((ttile[0]) + tilepos) //如果旋转后的位置（四个格）均没有被填满
		&& checkvalid((ttile[1]) + tilepos)
		&& checkvalid((ttile[2]) + tilepos)
		&& checkvalid((ttile[3]) + tilepos)
		&& (board[tempx[0]][tempy[0]] == false//如果移动后的位置（四个格）均没有被填满
		    && board[tempx[1]][tempy[1]] == false
			&& board[tempx[2]][tempy[2]] == false
			&& board[tempx[3]][tempy[3]] == false
			&& board[tempx[0]][tempy[1]] == false
			))
	{
		// 更新旋转，将当前方块设置为旋转之后的方块
		rotation = nextrotation;

		for (int i = 0; i < 4; i++)
			tile[i] = ttile[i];// 旋转成功，更新方块的当前位置

		updatetile();
	}
}

//////////////////////////////////////////////////////////////////////////
// 检查棋盘格在row行有没有被填充满

void checkfullrow(int row)
{
	int i,j,flag = 0;
	
	for (i = 0; i < 10; i++)
	{
		if (board[i][row] == true)
		{
			flag++;
		}
	}
	if (flag == 10)//如果在row行被填充满
	{
		for (i = 0; i < 10; i++)
		{
			//boardcolours[i] = black;//
									// 并将相应位置的颜色修改
			changecellcolour(vec2(i, row), black);//消掉一行
			//updatetile();
		}
		for (i = 0; i < 10; i++)
		{
			for (j = row + 1; j < 20; j++)//从row行起重新赋值
			{
                //上一行元素赋值给下一行元素
				
				// 并将相应位置的颜色修改
				if(board[i][j] == true)
					changecellcolour(vec2(i, j - 1), gray);
				else if(board[i][j] == false)
					changecellcolour(vec2(i, j - 1), black);
				
				board[i][j - 1] = board[i][j];

				// 给新方块赋上颜色
		
			}
		}row = 0;//重新从0开始遍历（从下往上），从而实现消掉多行的效果
	}
}
// 检查棋盘格有没有被填充满，遍历所有行
void checkFull()
{
	int row;
	for (row = 0; row < 20; row++)//从下往上遍历
	{
		checkfullrow(row);
		
	}
}
//判断是否满行，满则消除此行
bool isFull()
{
	int i, flag = 0;
	for (int row = 0; row < 20; row++)//从下往上遍历
	{
		flag = 0;
		for (i = 0; i < 10; i++)
		{
			if (board[i][row] == true)
			{
				flag++;
			}
		}
		if (flag == 10)//10个格子就消除
			break;
	}
	if (flag == 10)
		return true;
	return false;
}


//////////////////////////////////////////////////////////////////////////
// 放置当前方块，并且更新棋盘格对应位置顶点的颜色VBO

void settile()
{
	// 每个格子
	for (int i = 0; i < 4; i++)
	{
		// 获取格子在棋盘格上的坐标
		int x = (tile[i] + tilepos).x;
		int y = (tile[i] + tilepos).y;
		// 将格子对应在棋盘格上的位置设置为填充
		board[x][y] = true;

		// 并将相应位置的颜色修改
		changecellcolour(vec2(x,y), gray);//将颜色修改为gray！
	}
}


//////////////////////////////////////////////////////////////////////////
// 给定位置(x,y)，移动方块。有效的移动值为(-1,0)，(1,0)，(0,-1)，分别对应于向
// 左，向右和向下移动。如果移动成功，返回值为true，反之为false。

bool movetile(vec2 direction)
{
	int i;
	int tempx[4], tempy[4];
	// 计算移动之后的方块的位置坐标
	vec2 newtilepos[4];
	// 每个格子
	for (i = 0; i < 4; i++)
	{
		newtilepos[i] = tile[i] + tilepos + direction;
		tempx[i] = (tile[i] + tilepos).x+ direction.x;// 获取格子在棋盘格上的横坐标
		tempy[i] = (tile[i] + tilepos).y +direction.y;// 获取格子在棋盘格上的纵坐标
	}

	// 检查移动之后的有效性
	if (checkvalid(newtilepos[0])
		&& checkvalid(newtilepos[1])
		&& checkvalid(newtilepos[2])
		&& checkvalid(newtilepos[3])
		&&( board[tempx[0]][tempy[0]]==false//如果移动后的位置（四个格）均没有被填满
		&& board[tempx[1]][tempy[1]] == false
		&& board[tempx[2]][tempy[2]] == false
		&& board[tempx[3]][tempy[3]] == false
		&& board[tempx[0]][tempy[1]] == false
		
		))
		{
			// 有效：移动该方块
			tilepos.x = tilepos.x + direction.x;
			tilepos.y = tilepos.y + direction.y;

			updatetile();

			return true;
		}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// 游戏渲染部分

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(locxsize, xsize);
	glUniform1i(locysize, ysize);

	glBindVertexArray(vaoIDs[1]);
	glDrawArrays(GL_TRIANGLES, 0, 1200); // 绘制棋盘格 (10*20*2 = 400 个三角形)

	glBindVertexArray(vaoIDs[2]);
	glDrawArrays(GL_TRIANGLES, 0, 24);	 // 绘制当前方块 (8 个三角形)

	glBindVertexArray(vaoIDs[0]);
	glDrawArrays(GL_LINES, 0, 64);		 // 绘制棋盘格的线


	glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////
// 在窗口被拉伸的时候，控制棋盘格的大小，使之保持固定的比例。

void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}


//////////////////////////////////////////////////////////////////////////
// 键盘响应事件中的特殊按键响应

void special(int key, int x, int y)
{
	if(!gameover)
	{
		switch(key)
		{
			case GLUT_KEY_UP:	// 向上按键旋转方块
				rotate();
				break;
			case GLUT_KEY_DOWN: // 向下按键移动方块
				if (!movetile(vec2(0, -1)))//不能再往下时：调用settile()函数固定方块位置并开始生成新的方块
				{
					settile();
					newtile();
				}
				break;
			case GLUT_KEY_LEFT:  // 向左按键移动方块
				movetile(vec2(-1, 0));
				break;
			case GLUT_KEY_RIGHT: // 向右按键移动方块
				movetile(vec2(1, 0));
				break;
		}
	}
}


//提示信息
void printhelp()
{
	printf("%s\n\n", "操作提示");
	printf("'d' - 加速降落\n's' - 减速降落\n'a' - 维持初始速度\n'r' - 重启游戏\n'q或Esc' - exit\n");
}

//////////////////////////////////////////////////////////////////////////

void idle(void)
{
	glutPostRedisplay();
}

//设置方块自动下落
void down(int id)
{
	//staytime = 1000;
	if (!movetile(vec2(0, -1)) )
	{
		settile();
		newtile();
	}
	
	if(isFull())
		checkFull();
	glutTimerFunc(staytime, down, 1);//需要在主函数中再次调用，才能完成循环。//递归？
		
}
//////////////////////////////////////////////////////////////////////////
// 重新启动游戏

void restart()
{
	glewInit();
	init();
	glutTimerFunc(staytime, down, 1);//调用定时器

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
//	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();
}
//////////////////////////////////////////////////////////////////////////
// 键盘响应时间中的普通按键响应

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 033: // ESC键 和 'q' 键退出游戏
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit(EXIT_SUCCESS);
		break;
	case 'r': // 'r' 键重启游戏
		restart();
		break;
	case 'd':
		if (staytime>100)
			staytime -= 100;//加速降落
		break;
	case 's':
		if (staytime<3000)
			staytime += 200;//减速降落
		break;
	case 'a':
		staytime = 1000;//维持初始速度

	}
	glutPostRedisplay();//标记当前窗口需要重新绘制!!!!!!!
}

//////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	printhelp();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(xsize, ysize);
	glutInitWindowPosition(680, 178);
	glutCreateWindow("2014150004_魏依馨_期中大作业");
	glewInit();
	init();

	glutTimerFunc(staytime, down, 1);//调用定时器

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
