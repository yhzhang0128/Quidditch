#ifndef _GAME_LOGIC_H_
#define _GAME_LOGIC_H_

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <Windows.h>
#include <GL\glut.h>
#include "QuidditchLogic.hpp"

namespace GameLogic{

	const int WINDOW_WIDTH = 1080;
	const int WINDOW_HEIGHT = 720;
	const int FRAME_DELTA_TIME = 30;

	enum GameState
	{
		LoadingState,
		MenuState,
		GamingState,
		PanelState
	};
	GameState nowState;
	unsigned int frameNumber;

	void timer();
	void reshape(int w, int h);
	void moveMouse(int x, int y);
	void pressNormalKey(unsigned char key, int x, int y);
	void pressSpecialKey(int key, int x, int y);

	void initOpenGL(int argc, char** argv){
		// 初始化glut
		glutInit(&argc, argv);
		// 使用双缓冲
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
		// 窗口大小
		glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		// 窗口大小
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		// 窗口位置
		glutInitWindowPosition(50, 50);
		// 窗口Title
		glutCreateWindow("Quidditch Ball Game");

		// 每帧刷新函数
		glutDisplayFunc(timer);
		glutReshapeFunc(reshape);
		// 键盘函数
		glutKeyboardFunc(pressNormalKey);
		glutSpecialFunc(pressSpecialKey);
		// 鼠标函数
		glutPassiveMotionFunc(moveMouse);

		// 颜色不渐变
		glShadeModel(GL_FLAT);
		// 开启深度测试
		glEnable(GL_DEPTH_TEST);

		glClearColor(0.0, 0.0, 0.0, 0.0);
		// 当前涂色颜色是绿色
		glColor3f(0.0f, 1.0f, 0.0f);
		// 指定操作的目标矩阵
		glMatrixMode(GL_PROJECTION);
		// 重置矩阵为单位矩阵
		glLoadIdentity();
	}

	void timer_callback(int nouse);
	void timer(){
		frameNumber++;
		// TODO
		// 当前只有游戏逻辑，所以直接调用Quidditch::frameDisplay()
		// 后期版本此处应为状态机
		Quidditch::frameDisplay();

		glutTimerFunc(FRAME_DELTA_TIME, timer_callback, 1);
	}
	void timer_callback(int nouse){
		timer();
	}

	void pressNormalKey(unsigned char key, int x, int y){
		Quidditch::pressNormalKey(key, x, y);
	}

	void pressSpecialKey(int key, int x, int y){
		Quidditch::pressSpecialKey(key, x, y);
	}

	void moveMouse(int x, int y){
		//printf("%d %d\n", x, y);
		Quidditch::moveMouse(x, y);
	}

	void start(int argc, char** argv){
		initOpenGL(argc, argv);
		// 初始化魁地奇游戏逻辑
		Quidditch::init();
		// TODO
		// 后期版本中会加入其他界面(比如菜单、进度条)的初始化，预读取等操作

		glutMainLoop();
	}

	void reshape(int w, int h)
	{
		glViewport(0, 0, (GLsizei)w, (GLsizei)h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}

}
#endif