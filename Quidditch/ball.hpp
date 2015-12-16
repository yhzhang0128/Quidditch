#ifndef _BALL_H_
#define _BALL_H_

#include "Vertex3D.h"
#include "Texture.hpp"
#include "Model3D.hpp"
#include <ctime>
#include <stdlib.h>
#include <GL\glut.h>

#define MOTHER_BALL 0
#define ENEMY_BALL 1
#define TARGET_BALL 2
#define MAGIC_BALL 3

#define DESK_HEIGHT 25

#define DESK_EDGE_Z 340
#define DESK_EDGE_X 715

class Ball{
private:
	enum BallType{
		MotherBall = 0,
		EnemyBall,
		TargetBall,
		MagicBall
	} type;
	double radius, angle;
	int magicHeight;
	Vertex3D location, speed;

	// 出界检测
	bool collideZ(){
		return (location.z > DESK_EDGE_Z - radius) || (location.z < radius - DESK_EDGE_Z);
	}
	bool collideX(){
		return (location.x > DESK_EDGE_X - radius) || (location.x < radius - DESK_EDGE_X);
	}

	void roundUp(){
		if (location.z > DESK_EDGE_Z - radius) location.z = DESK_EDGE_Z - radius;
		if (location.z < radius - DESK_EDGE_Z) location.z = radius - DESK_EDGE_Z;
		if (location.x > DESK_EDGE_X - radius) location.x = DESK_EDGE_X - radius;
		if (location.x < radius - DESK_EDGE_X) location.x = radius - DESK_EDGE_X;
	}

public:
	Ball(){
		//		location.x = -715;
		//		location.y = 35;
		//		location.z = 340;

		radius = 10;
	}
	Ball(int tp, Vertex3D pos, Vertex3D iniSpeed){
		if (tp == MOTHER_BALL)
			type = MotherBall;
		if (tp == ENEMY_BALL)
			type = EnemyBall;
		if (tp == TARGET_BALL)
			type = TargetBall;
		if (tp == MAGIC_BALL)
			type = MagicBall;

		radius = 30;
		location = pos;
		speed = iniSpeed;
		angle = 0;

		if (type == MagicBall){
			speed.y = 8;
			magicHeight = 40;
		}

		srand(time(0));
	}

	// 碰撞检测
	void collideTest(Ball& other, int& HP){
		double dist = vertexDist(location, other.location);

		if (dist < radius + other.radius){
			if (type == MOTHER_BALL){
				double speedScalar = sqrt(other.getSpeed().squareLength());
				if (speedScalar < NEAR_ZERO_ESP) speedScalar = sqrt(speed.squareLength());
				Vertex3D delta = other.location - location;
				delta.normalize();
				delta *= speedScalar;
				other.setSpeed(delta);

				if (other.type == ENEMY_BALL) HP -= 10;
				//return;
			}
			else if (other.type == MOTHER_BALL){
				double speedScalar = sqrt(speed.squareLength());
				if (speedScalar < NEAR_ZERO_ESP) speedScalar = sqrt(other.speed.squareLength());
				speed = location - other.location;
				speed.normalize();
				speed *= speedScalar;

				if (type == ENEMY_BALL) HP -= 10;
				//return;
			}

			else{
				// 两个球都不是母球
				//if (type == TargetBall || other.type == TargetBall)
				//	printf("%lf   :  %lf\n", dist, radius + other.radius);
				Vertex3D tmp = speed;
				speed = other.speed;
				other.speed = tmp;

				if (speed.squareLength() < NEAR_ZERO_ESP){
					speed.x = -other.speed.x;
					speed.y = -other.speed.y;
					speed.z = -other.speed.z;
				}
				if (other.speed.squareLength() < NEAR_ZERO_ESP){
					other.speed.x = -speed.x;
					other.speed.y = -speed.y;
					other.speed.z = -speed.z;
				}
			}

			double ratio = (radius + other.radius) / dist;
			Vertex3D delta = (location - other.location);
			delta *= ratio; delta.x += 2; delta.z += 2;
			location = other.location + delta;

		}
	}

	// 移动
	void move(){
		if (type == MagicBall){
			location.y += speed.y;
			/*
			int t = rand() % 5;
			if (magicHeight == 100){
			if (t == 0) magicHeight = 0;
			}
			else{
			if (location.y < DESK_HEIGHT + 10 && t == 0)
			magicHeight = 100;
			}*/

			if (location.y > DESK_HEIGHT + 350 || location.y < DESK_HEIGHT + magicHeight)
				speed.y = -speed.y;
			//printf("%lf\n", location.z);
		}

		location.x += speed.x;
		location.y += speed.y;
		location.z += speed.z;

		if (collideZ()){
			roundUp();
			if (type != MOTHER_BALL)
				speed.z = -speed.z;
		}
		if (collideX()){
			roundUp();
			if (type != MOTHER_BALL)
				speed.x = -speed.x;
		}

	}

	void moveLeft(double scale){
		printf("%lf\n", scale);

		double speedScalar = sqrt(speed.squareLength());
		double x = speed.x, z = speed.z;
		speed.x = scale * x + z;
		speed.z = scale * z - x;
		speed.normalize();
		speed *= speedScalar;
	}

	void moveRight(int scale){
		double speedScalar = sqrt(speed.squareLength());
		double x = speed.x, z = speed.z;
		speed.x = scale * x - z;
		speed.z = scale * z + x;
		speed.normalize();
		speed *= speedScalar;
	}

	void Render(double ratio){
		glPushMatrix();

		// 移动和旋转
		glTranslated(location.x * ratio, location.y * ratio, location.z * ratio);

		glEnable(GL_TEXTURE_2D);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		switch (type){
		case MotherBall:
			glBindTexture(GL_TEXTURE_2D, Texture::textureNames[TEX_BALL_MOTHER]);
			break;
		case EnemyBall:
			glBindTexture(GL_TEXTURE_2D, Texture::textureNames[TEX_BALL_ENEMY]);
			break;
		case TargetBall:
			glBindTexture(GL_TEXTURE_2D, Texture::textureNames[TEX_BALL_TARGET]);
			break;
		case MagicBall:
			glBindTexture(GL_TEXTURE_2D, Texture::textureNames[TEX_BALL_MAGIC]);
			break;
		}

		glEnable(GL_TEXTURE_GEN_S);						// 自动生成s方向纹理坐标
		glEnable(GL_TEXTURE_GEN_T);


		//		glutSolidTeapot(15);
		//		glTranslated(0, 30, 0);

		glRotated(90, 1, 0, 0);
		//glutWireTeapot(15);
		glutSolidSphere(radius * ratio, 40, 40);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);						// 自动生成s方向纹理坐标
		glDisable(GL_TEXTURE_GEN_T);


		glPopMatrix();
	}

	// 速度的getter和setter
	Vertex3D getSpeed(){
		return speed;
	}
	void setSpeed(Vertex3D spd){
		speed = spd;
	}

	Vertex3D renderLocation(){
		double ratio = 0.35;
		return Vertex3D(location.x * ratio, location.y * ratio, location.z * ratio);
	}
};

#endif