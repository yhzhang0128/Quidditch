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
		// ��ʼ��glut
		glutInit(&argc, argv);
		// ʹ��˫����
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
		// ���ڴ�С
		glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		// ���ڴ�С
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		// ����λ��
		glutInitWindowPosition(50, 50);
		// ����Title
		glutCreateWindow("Quidditch Ball Game");

		// ÿ֡ˢ�º���
		glutDisplayFunc(timer);
		glutReshapeFunc(reshape);
		// ���̺���
		glutKeyboardFunc(pressNormalKey);
		glutSpecialFunc(pressSpecialKey);
		// ��꺯��
		glutPassiveMotionFunc(moveMouse);

		// ��ɫ������
		glShadeModel(GL_FLAT);
		// ������Ȳ���
		glEnable(GL_DEPTH_TEST);

		glClearColor(0.0, 0.0, 0.0, 0.0);
		// ��ǰͿɫ��ɫ����ɫ
		glColor3f(0.0f, 1.0f, 0.0f);
		// ָ��������Ŀ�����
		glMatrixMode(GL_PROJECTION);
		// ���þ���Ϊ��λ����
		glLoadIdentity();
	}

	void timer_callback(int nouse);
	void timer(){
		frameNumber++;
		// TODO
		// ��ǰֻ����Ϸ�߼�������ֱ�ӵ���Quidditch::frameDisplay()
		// ���ڰ汾�˴�ӦΪ״̬��
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
		// ��ʼ����������Ϸ�߼�
		Quidditch::init();
		// TODO
		// ���ڰ汾�л������������(����˵���������)�ĳ�ʼ����Ԥ��ȡ�Ȳ���

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