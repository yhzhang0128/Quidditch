#ifndef _QUIDDITCH_LOGIC_H_
#define _QUIDDITCH_LOGIC_H_

#include <cstdio>
#include <GL\glut.h>
#include "ImageLoader.hpp"
#include "Ball.hpp"
#include "Model3D.hpp"
#include "Texture.hpp"
#include "ClothSimulation.hpp"
using ClothSimulation::wind;

namespace Quidditch{

	int HP;

	// �õ��ĸ���ģ��
	Model3D::Model3D desk;// , ball;
	int wave_step, wave_angle;
	ClothSimulation::Banner banner0(16, 9, 16, 9, TEX_BANNER0, PIXMAP_BANNER0);
	ClothSimulation::Banner banner1(16, 9, 16, 9, TEX_BANNER1, PIXMAP_BANNER1);
	Ball motherBall(MOTHER_BALL, Vertex3D(0, DESK_HEIGHT, 0), Vertex3D(10, 0, 10));
	Ball magicBall(MAGIC_BALL, Vertex3D(0, DESK_HEIGHT + 100, 0), Vertex3D(10, 0, 10));

	const int ENEMY_BALL_NUM = 3;
	const int TARGET_BALL_NUM = 3;
	const int BALL_NUM = ENEMY_BALL_NUM + TARGET_BALL_NUM + 1;
	Ball enemyBall[ENEMY_BALL_NUM] = {
		Ball(ENEMY_BALL, Vertex3D(100, DESK_HEIGHT, 100), Vertex3D(13, 0, 6)),
		Ball(ENEMY_BALL, Vertex3D(100, DESK_HEIGHT, -100), Vertex3D(3, 0, 20)),
		Ball(ENEMY_BALL, Vertex3D(-100, DESK_HEIGHT, 100), Vertex3D(9, 0, 12)),
	};
	Ball targetBall[TARGET_BALL_NUM] = {
		Ball(TARGET_BALL, Vertex3D(400, DESK_HEIGHT, 150), Vertex3D(0, 0, 0)),
		Ball(TARGET_BALL, Vertex3D(400, DESK_HEIGHT, 0), Vertex3D(0, 0, 0)),
		Ball(TARGET_BALL, Vertex3D(400, DESK_HEIGHT, -150), Vertex3D(0, 0, 0)),
	};

	Ball* allBall[1 + ENEMY_BALL_NUM + TARGET_BALL_NUM];

	// ����������Ϣ
	struct LookAt{
		GLdouble eyex, eyey, eyez;
		GLdouble centerx, centery, centerz;
		GLdouble upx, upy, upz;
	} lookAt;

	// ��GameLogic��glutMainloopǰ����
	void init(){
		HP = 1000;

		lookAt.eyex = -130; lookAt.eyey = 150; lookAt.eyez = 302;
		lookAt.centerx = 0; lookAt.centery = 0;  lookAt.centerz = 0;
		lookAt.upx = lookAt.upz = 0.0; lookAt.upy = 1.0;

		// ׼����������
		Texture::prepareDesk();
		// ׼������ģ��     
		desk.parseFile("objects/desk2_simple.obj", MODEL_TYPE_DESK);
		// ׼������������
		Texture::prepareBall();
		//ball.parseFile("objects/ball.obj", MODEL_TYPE_BALL);
		// ׼����������
		Texture::prepareBanner();
		wave_angle = 0; wave_step = 2;
		wind.x = -2; wind.y = 0; wind.z = -4;

		// ׼����������
		Texture::prepareRoom();

		// ��ײ���׼��
		allBall[0] = &motherBall;
		allBall[1] = enemyBall + 0;
		allBall[2] = enemyBall + 1;
		allBall[3] = enemyBall + 2;
		allBall[4] = targetBall + 0;
		allBall[5] = targetBall + 1;
		allBall[6] = targetBall + 2;
	}

	// ����������
	void drawRoom();
	void drawString(const char* str);

	// ����ÿһ֡
	void frameDisplay(){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, (GLfloat)900 / (GLfloat)600, 0.1f, 5000.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();


		// ��ײ���
		for (int i = 0; i < BALL_NUM - 1; i++)
			for (int j = i + 1; j < BALL_NUM; j++)
				allBall[i]->collideTest(*allBall[j], HP);

		// ���ƶ�
		motherBall.move();
		for (int i = 0; i < ENEMY_BALL_NUM; i++)
			enemyBall[i].move();
		for (int i = 0; i < TARGET_BALL_NUM; i++)
			targetBall[i].move();
		magicBall.move();


		// �����λ��
		Vertex3D motherLocation = motherBall.renderLocation();

		Vertex3D delta = motherBall.getSpeed().normalize();
		delta *= 200;
		Vertex3D eye = motherLocation - delta;

		gluLookAt(eye.x, lookAt.eyey, eye.z,
			//lookAt.eyex, lookAt.eyey, lookAt.eyez,
			motherLocation.x, motherLocation.y, motherLocation.z,
			//lookAt.centerx, lookAt.centery, lookAt.centerz, 
			lookAt.upx, lookAt.upy, lookAt.upz);

		// ����
		/*
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);
		//
		static GLfloat noLight[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static GLfloat ambientLight[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
		static GLfloat diffuseLight[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
		static GLfloat brightLight[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		static GLfloat lightPos[] = { 45.0f, 45.0f, 45.0f, 1.0f };
		// Light0
		glEnable(GL_LIGHTING);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, noLight);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
		glLightfv(GL_LIGHT0, GL_SPECULAR, brightLight);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		glEnable(GL_LIGHT0);
		
		//������ɫ׷��
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseLight);
		glMaterialfv(GL_BACK, GL_DIFFUSE, diffuseLight);
		//glMaterialfv(GL_FRONT, GL_SPECULAR, brightLight);
		*/

		// ������ֵ
		glColor3f(1, 1, 1);
		//glRasterPos2f(motherLocation.x, motherLocation.y);
		glRasterPos3f(motherLocation.x, motherLocation.y + 30, motherLocation.z);
		char buf[64];
		sprintf_s(buf, "HP: %d", HP);
		drawString(buf);

		// ������
		//drawRoom();

		// ������
		desk.Render(Vertex3D(0, 0, 0), 0.35);

		// ��ĸ��
		motherBall.Render(0.35);

		// ��������
		for (int i = 0; i < ENEMY_BALL_NUM; i++)
			enemyBall[i].Render(0.35);

		// ��Ŀ����
		for (int i = 0; i < TARGET_BALL_NUM; i++)
			targetBall[i].Render(0.35);

		// ����ɫ����
		magicBall.Render(0.35);
		
		glPushMatrix();
		glTranslatef(-290, 20, 0);
		glRotatef(180, 0, 1, 0);
		glScaled(10, 10, 10);
		banner1.addForce(Vertex3D(0, -0.3, 0) * ClothSimulation::STEP_TIMESQUARE); // add gravity each frame, pointing down
		banner1.windForce(wind * -ClothSimulation::STEP_TIMESQUARE); // generate some wind each frame
		banner1.rotateZ(wave_step);
		banner1.stepMove();
		banner1.drawNurbsVersion();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(320, 0, 0);
		//glRotatef(180, 0, 1, 0);
		glScaled(10, 10, 10);
		banner0.addForce(Vertex3D(0, -0.3, 0) * ClothSimulation::STEP_TIMESQUARE); // add gravity each frame, pointing down
		banner0.windForce(wind * ClothSimulation::STEP_TIMESQUARE); // generate some wind each frame
		//banner0.rotateZ(wave_step);
		banner0.stepMove();
		banner0.drawNurbsVersion();
		glPopMatrix();

		if (abs(wave_angle += wave_step) > 30)
			wave_step = -wave_step;

		// ��������֡
		glutSwapBuffers();
	}

	// �����¼���Ӧ
	bool detectMouse;
	void pressNormalKey(int key, int x, int y){
		switch (key){
		case 'a':
			wind = Vertex3D(wind.x * cosf(0.05) - wind.z * sinf(0.05), wind.y, wind.z * cosf(0.05) + wind.x * sinf(0.05));
			break;
		case 'd':
			wind = Vertex3D(wind.x * cosf(0.05) + wind.z * sinf(0.05), wind.y, wind.z * cosf(0.05) - wind.x * sinf(0.05));
			break;
		case'w':
			if (wind.length() < 11)
				wind *= 1.1;
			break;
		case 's':
			if (wind.length() > 0.005)
				wind *= 0.9;
			break;
		case ' ':
			detectMouse = !detectMouse;
			break;
		}
	}
	void pressSpecialKey(int key, int x, int y){
		/*
		switch (key){
		case GLUT_KEY_UP:
			printf("UP\n");
			break;
		case GLUT_KEY_DOWN:
			printf("DOWN\n");
			break;
		case GLUT_KEY_LEFT:
			printf("LEFT\n");
			motherBall.moveLeft(20);
			break;
		case GLUT_KEY_RIGHT:
			motherBall.moveRight(20);
			printf("RIGHT\n");
			break;
		}*/
		switch (key){
		case GLUT_KEY_UP:
			banner0.rotateX(1);
			banner1.rotateX(1);
			break;
		case GLUT_KEY_DOWN:
			banner0.rotateX(-1);
			banner1.rotateX(-1);
			break;
		case GLUT_KEY_LEFT:
			banner0.rotateZ(-1);
			banner1.rotateZ(-1);
			break;
		case GLUT_KEY_RIGHT:
			banner0.rotateZ(1);
			banner1.rotateZ(1);
			break;
		}
	}

	void moveMouse(int x, int y){
		//if (!detectMouse)
			//return;

		static int prevx, prevy;
		if (prevx == 0 && prevy == 0){
			prevx = x; prevy = y;  return;
		}
		else{
			if (x < prevx)
				motherBall.moveLeft(100.0 / (double)(prevx - x));
			if (x > prevx)
				motherBall.moveRight(100.0 / (double)(x - prevx));
			prevx = x;

			if (y < prevy && lookAt.eyey > 70)
				lookAt.eyey -= (prevy - y);
			if (y > prevy && lookAt.eyey < 300)
				lookAt.eyey += (y - prevy);
			prevy = y;
		}
	}


	void drawRoom(){
		glPushMatrix();
		glTranslated(0.0, -40.0, 0.0);

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, Texture::textureNames[TEX_FLOOR]);

		glBegin(GL_QUADS);
		glTexCoord2f(5, 0);  glVertex3f(512, 0, 512);
		glTexCoord2f(5, 5);  glVertex3f(512, 0, -512);
		glTexCoord2f(0, 5);  glVertex3f(-512, 0, -512);
		glTexCoord2f(0, 0);  glVertex3f(-512, 0, 512);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, Texture::textureNames[TEX_WALL]);
		double relativeH = Texture::bigPixmaps[PIXMAP_WALL].relativeH;

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);  glVertex3f(512, 340, 512);
		glTexCoord2f(0, relativeH);  glVertex3f(512, 0, 512);
		glTexCoord2f(relativeH, relativeH);  glVertex3f(-512, 0, 512);
		glTexCoord2f(relativeH, 0);  glVertex3f(-512, 340, 512);
		glEnd();

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);  glVertex3f(512, 340, -512);
		glTexCoord2f(0, relativeH);  glVertex3f(512, 0, -512);
		glTexCoord2f(relativeH, relativeH);  glVertex3f(512, 0, 512);
		glTexCoord2f(relativeH, 0);  glVertex3f(512, 340, 512);
		glEnd();

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);  glVertex3f(-512, 340, -512);
		glTexCoord2f(0, relativeH);  glVertex3f(-512, 0, -512);
		glTexCoord2f(relativeH, relativeH);  glVertex3f(512, 0, -512);
		glTexCoord2f(relativeH, 0);  glVertex3f(512, 340, -512);
		glEnd();

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);  glVertex3f(-512, 340, 512);
		glTexCoord2f(0, relativeH);  glVertex3f(-512, 0, 512);
		glTexCoord2f(relativeH, relativeH);  glVertex3f(-512, 0, -512);
		glTexCoord2f(relativeH, 0);  glVertex3f(-512, 340, -512);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glPopMatrix();

	}

	// ��������
	void axis(double length){
		glPushMatrix();
		glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, length);
		glEnd();
		glTranslated(0.0, 0.0, length - 0.2);
		glutWireCone(10, 40, 8, 8);
		glPopMatrix();
	}

	void drawAxis(){
		glColor3f(1.0f, 0.0f, 0.0f);
		axis(80);  // z

		glPushMatrix();
		glRotated(90.0, 0, 1.0, 0);
		axis(80); //x
		glPopMatrix();

		glPushMatrix();
		glRotated(-90.0, 1.0, 0.0, 0.0);
		axis(80); //y
		glPopMatrix();
	}

	void drawString(const char* str) {
		static int isFirstCall = 1;
		static GLuint lists;

		if (isFirstCall) { // ����ǵ�һ�ε��ã�ִ�г�ʼ��
			// Ϊÿһ��ASCII�ַ�����һ����ʾ�б�
			isFirstCall = 0;

			// ����MAX_CHAR����������ʾ�б���
			lists = glGenLists(128);

			// ��ÿ���ַ��Ļ������װ����Ӧ����ʾ�б���
			wglUseFontBitmaps(wglGetCurrentDC(), 0, 128, lists);
		}
		// ����ÿ���ַ���Ӧ����ʾ�б�����ÿ���ַ�
		for (; *str != '\0'; ++str)
			glCallList(lists + *str);
	}

}

#endif