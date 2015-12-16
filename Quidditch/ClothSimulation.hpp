#ifndef _CLOTH_SIMULATION_H_
#define _CLOTH_SIMULATION_H_

#include "Vertex3D.h"
#include <vector>
#include <GL\glut.h>

namespace ClothSimulation{
	Vertex3D wind;

	const float STEP_TIMESQUARE = 0.35 * 0.35;
	const float DAMPING = 0.01;
	const float PARTICLE_MASS = 2;
	const float CONSTRAINT_ITERATIONS = 15;

	class Particle{
	private:
		bool fixed;

		float mass;
		Vertex3D currentPosition, previousPosition;  // µ±Ç°Î»ÖÃ£¬ ÉÏÒ»Ö¡Î»ÖÃ
		Vertex3D normal, acceleration;
		Vertex3D texturePosition;
	public:
		Particle()	{}
		Particle(const Vertex3D &pos, float m) :
			currentPosition(pos), previousPosition(pos),
			acceleration(Vertex3D(0, 0, 0)), mass(m),
			fixed(false), normal(Vertex3D(0, 0, 0))	{}

		void addForce(Vertex3D force){
			acceleration += force / mass;
		}

		void stepMove(){
			if (!fixed) {
				Vertex3D tmp(currentPosition);
				currentPosition = currentPosition + (currentPosition - previousPosition) * (1.0 - DAMPING) + acceleration * STEP_TIMESQUARE;
				previousPosition = tmp;
				resetAcceleration();
			}
		}

		inline Vertex3D& getCurrentPosition()	{ return currentPosition; }
		void resetAcceleration()	{ acceleration = Vertex3D(0, 0, 0); }
		void offsetPosition(const Vertex3D& v)	{ if (!fixed) currentPosition += v; }
		void setPosition(const Vertex3D& v)	{ currentPosition = previousPosition = v; }
		void setFixed()	{ fixed = true; }
		void setNormal(Vertex3D norm)	{ normal = norm; }
		inline Vertex3D& getNormal()	{ return normal; }

		Vertex3D getTexturePosition()	{ return texturePosition; }
		void setTexturePosition(float x, float y)	{ texturePosition.x = x; texturePosition.y = y; }
	};

	class Constraint{
	private:
		float relaxDistance;
	public:
		Particle *p1, *p2;
		Constraint(Particle *p1, Particle *p2) : p1(p1), p2(p2)
		{
			Vertex3D vec = p1->getCurrentPosition() - p2->getCurrentPosition();
			relaxDistance = vec.length();
		}

		void stepMove(){
			// delta is the vector:  p1 -> p2
			Vertex3D delta = p2->getCurrentPosition() - p1->getCurrentPosition();
			float currentDistance = delta.length();
			Vertex3D stepVector = delta * (1 - relaxDistance / currentDistance);
			p1->offsetPosition(stepVector * 0.5);
			p2->offsetPosition(stepVector * -0.5);
		}
	};

	class Banner{
	private:
		int pixmap_id, texture_id;
		float draw_width, draw_height;
		int width_granularity, height_granularity;
		std::vector<Particle> particles;
		std::vector<Constraint> constraints;

		inline Particle* getParticle(int x, int y)	{ return &particles[y * width_granularity + x]; }
		void bindConstraint(Particle *p1, Particle *p2)	{ constraints.push_back(Constraint(p1, p2)); }
		// TODO maybe only used for triangle
		Vertex3D calcTriangleNormal(Particle *p1, Particle *p2, Particle *p3){
			Vertex3D v1 = p2->getCurrentPosition() - p1->getCurrentPosition();
			Vertex3D v2 = p3->getCurrentPosition() - p1->getCurrentPosition();
			return v1.cross(v2);
		}

		// ÒòÎªÈý½ÇÐÎ¾ö¶¨Ò»¸öÆ½Ãæ£¬¼´Ò»¸ö·¨ÏòÁ¿£¬ÓÃ·¨ÏòÁ¿ºÍ·çÏòµã»ý
		void addWindForce(Particle *p1, Particle *p2, Particle *p3, const Vertex3D direction){
			Vertex3D normal = calcTriangleNormal(p1, p2, p3);
			Vertex3D d = normal.normalize();
			Vertex3D force = normal*(d.dot(direction));
			p1->addForce(force);
			p2->addForce(force);
			p3->addForce(force);
		}

		void drawTriangle(Particle *p1, Particle *p2, Particle *p3)
		{
			glColor3f(1, 1, 1);

			float relativeW = Texture::smallPixmaps[pixmap_id].relativeW;
			float relativeH = Texture::smallPixmaps[pixmap_id].relativeH;

			glTexCoord2f(relativeW * p1->getTexturePosition().x,
				relativeH * p1->getTexturePosition().y);
			glNormal3f(p1->getNormal().normalize().x, p1->getNormal().normalize().y, p1->getNormal().normalize().z);
			glVertex3f(p1->getCurrentPosition().x, p1->getCurrentPosition().y, p1->getCurrentPosition().z);

			glTexCoord2f(relativeW * p2->getTexturePosition().x,
				relativeH * p2->getTexturePosition().y);
			glNormal3f(p2->getNormal().normalize().x, p2->getNormal().normalize().y, p2->getNormal().normalize().z);
			glVertex3f(p2->getCurrentPosition().x, p2->getCurrentPosition().y, p2->getCurrentPosition().z);

			glTexCoord2f(relativeW * p3->getTexturePosition().x,
				relativeH * p3->getTexturePosition().y);
			glNormal3f(p3->getNormal().normalize().x, p3->getNormal().normalize().y, p3->getNormal().normalize().z);
			glVertex3f(p3->getCurrentPosition().x, p3->getCurrentPosition().y, p3->getCurrentPosition().z);
		}

		void bindAdjacentConstraints(){
			for (int x = 0; x < width_granularity; x++)
			{
				for (int y = 0; y < height_granularity; y++)
				{
					if (x < width_granularity - 1)
						bindConstraint(getParticle(x, y), getParticle(x + 1, y));
					if (y < height_granularity - 1)
						bindConstraint(getParticle(x, y), getParticle(x, y + 1));
					if (x < width_granularity - 1 && y<height_granularity - 1)
						bindConstraint(getParticle(x, y), getParticle(x + 1, y + 1));
					if (x < width_granularity - 1 && y<height_granularity - 1)
						bindConstraint(getParticle(x + 1, y), getParticle(x, y + 1));

					if (x < width_granularity - 2)
						bindConstraint(getParticle(x, y), getParticle(x + 2, y));
					if (y < height_granularity - 2)
						bindConstraint(getParticle(x, y), getParticle(x, y + 2));
					if (x < width_granularity - 2 && y < height_granularity - 2)
						bindConstraint(getParticle(x, y), getParticle(x + 2, y + 2));
					if (x < width_granularity - 2 && y < height_granularity - 2)
						bindConstraint(getParticle(x + 2, y), getParticle(x, y + 2));

				}
			}
		}

		void renderStart(){
			glShadeModel(GL_SMOOTH);
			glEnable(GL_LIGHTING);

			glEnable(GL_LIGHT0);
			GLfloat lightPos[4] = { 1.0, 0.0, 0.0, 0.0 };
			glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat *)&lightPos);

			glEnable(GL_LIGHT1);
			GLfloat lightAmbient1[4] = { 0.7, 0.7, 0.7, 0.0 };
			GLfloat lightPos1[4] = { -1.0, -1.0, -1.0, 0.0 };
			GLfloat lightDiffuse1[4] = { 0.8, 0.8, 0.8, 0.0 };

			glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
			glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient1);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);

			glMaterialfv(GL_FRONT, GL_DIFFUSE, lightDiffuse1);

			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

			// draw pole
			glColor3f(1, 1, 1);
			glPushMatrix();
			//glRotatef(90, -1, 0, 0);

			Vertex3D raw(0, 0, 1);
			Particle *st = getParticle(0, 0);
			Particle *ed = getParticle(0, 1);
			Vertex3D target = ed->getCurrentPosition() - st->getCurrentPosition();
			//printf("target: %f %f %f\n", target.x, target.y, target.z);

			Vertex3D rot = raw.cross(target);
			//printf("rot: %f %f %f\n", rot.x, rot.y, rot.z);

			float angle = 180 * acos(raw.dot(target) / raw.length() / target.length()) / PI;
			//printf("angle: %f\n", angle);
			glRotatef(angle, rot.x, rot.y, rot.z);

			GLUquadricObj *quadratic;
			quadratic = gluNewQuadric();


			gluQuadricNormals(quadratic, GL_SMOOTH);
			gluQuadricTexture(quadratic, TRUE);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, Texture::textureNames[TEX_DESK_BORDER]);
			gluCylinder(quadratic, 0.15f, 0.15f, draw_height*1.5, 32, 32);
			glDisable(GL_TEXTURE_2D);

			glPopMatrix();

			// Texture is modulate so that color should be white

			glPushMatrix();
			target *= draw_height * 0.5;
			glTranslatef(target.x, target.y, target.z);
		}

		void renderEnd(){
			glPopMatrix();

			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHTING);
		}
	public:
		Banner(float width, float height, int width_gran, int height_gran, int texture, int pixmap) :
			pixmap_id(pixmap), texture_id(texture),
			width_granularity(width_gran), height_granularity(height_gran){

			draw_width = width;
			draw_height = height;
			particles.resize(width_granularity * height_granularity);
			for (int x = 0; x < width_granularity; x++)
				for (int y = 0; y < height_granularity; y++){
					// ÔÚYOZÆ½ÃæÉÏ£¬YÖáÕý·½Ïò£¬zÖá¸º·½Ïò
					Vertex3D pos(0, height * (y / (float)height_granularity), -width * (x / (float)width_granularity));

					particles[y * width_granularity + x] = Particle(pos, PARTICLE_MASS);
					if (x == 0)
						particles[y * width_granularity + x].setFixed();

					// Set Texture
					particles[y * width_granularity + x].setTexturePosition((float)x / (width_granularity - 1), (float)y / (height_granularity - 1));
				}

			bindAdjacentConstraints();


			bufferNURBS = new GLfloat[width_granularity * height_granularity * 3];
		}


		void drawBallVersion(){
			renderStart();
			glColor3f(0, 0, 1);
			for (int i = 0; i < particles.size(); i++){
				Vertex3D pos(particles[i].getCurrentPosition());

				glPushMatrix();
				glTranslated(pos.x, pos.y, pos.z);

				glutSolidSphere(0.1, 30, 30);
				glPopMatrix();
			}
			renderEnd();
		}

		/*
		(x,y)   *--* (x+1,y)
		| /|
		|/ |
		(x,y+1) *--* (x+1,y+1)
		*/
		void drawTriangleVersion(){
			renderStart();

			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D, Texture::textureNames[texture_id]);

			glBegin(GL_TRIANGLES);
			for (int x = 0; x < width_granularity - 1; x++){
				for (int y = 0; y < height_granularity - 1; y++){
					// Count Normal
					Vertex3D normalTriangle1 = calcTriangleNormal(getParticle(x, y), getParticle(x + 1, y), getParticle(x, y + 1));
					Vertex3D normalTriangle2 = calcTriangleNormal(getParticle(x + 1, y + 1), getParticle(x, y + 1), getParticle(x + 1, y));

					getParticle(x, y)->setNormal(normalTriangle1);
					getParticle(x + 1, y)->setNormal(normalTriangle1 + normalTriangle2);
					getParticle(x, y + 1)->setNormal(normalTriangle1 + normalTriangle2);
					getParticle(x + 1, y + 1)->setNormal(normalTriangle2);

					// Draw Triangle
					drawTriangle(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1));
					drawTriangle(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1));
				}
			}
			glEnd();

			glDisable(GL_TEXTURE_2D);
			renderEnd();
		}


		GLfloat *bufferNURBS;
		void drawNurbsVersion(){
			for (int i = 0; i < width_granularity; i++)
				for (int j = 0; j < height_granularity; j++){
					bufferNURBS[(i * height_granularity + j) * 3 + 0] = getParticle(i, j)->getCurrentPosition().x;
					bufferNURBS[(i * height_granularity + j) * 3 + 1] = getParticle(i, j)->getCurrentPosition().y;
					bufferNURBS[(i * height_granularity + j) * 3 + 2] = getParticle(i, j)->getCurrentPosition().z;
				}

			renderStart();

			glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, height_granularity,
				0, 1, 3 * height_granularity, width_granularity, bufferNURBS);

			GLfloat texpts[2][2][2] = { { { 0.0, 0.0 }, { 0.0, Texture::smallPixmaps[pixmap_id].relativeH } },
			{ { Texture::smallPixmaps[pixmap_id].relativeW, 0.0 }, { Texture::smallPixmaps[pixmap_id].relativeW, Texture::smallPixmaps[pixmap_id].relativeH } } };
			glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, //
				0, 1, 4, 2, &texpts[0][0][0]);

			glEnable(GL_MAP2_VERTEX_3);
			glEnable(GL_MAP2_TEXTURE_COORD_2);
			glEnable(GL_AUTO_NORMAL);
			glEnable(GL_NORMALIZE);

			glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D, Texture::textureNames[texture_id]);

			glEnable(GL_TEXTURE_2D);
			glEvalMesh2(GL_FILL, 0, 20, 0, 20);

			glDisable(GL_AUTO_NORMAL);
			glDisable(GL_NORMALIZE);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_MAP2_VERTEX_3);
			glDisable(GL_MAP2_TEXTURE_COORD_2);

			renderEnd();
		}

		void stepMove(){
			// Calculate Adjacent Constraint
			for (int i = 0; i < CONSTRAINT_ITERATIONS; i++)
				for (int j = 0; j < constraints.size(); j++)
					constraints[j].stepMove();

			for (int i = 0; i < particles.size(); i++)
				particles[i].stepMove();
		}

		void addForce(const Vertex3D& direction){
			for (int i = 0; i < particles.size(); i++)
				particles[i].addForce(direction);
		}

		void windForce(const Vertex3D direction)
		{
			for (int x = 0; x < width_granularity - 1; x++)
			{
				for (int y = 0; y < height_granularity - 1; y++)
				{
					addWindForce(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1), direction);
					addWindForce(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1), direction);
				}
			}
		}

		void rotateX(float angle){
			Particle *st = getParticle(0, 0);
			Particle *ed = getParticle(0, 1);

			angle = angle * 2 * PI / 360;

			float real = cosf(angle), complex = sinf(angle);
			Vertex3D prev = ed->getCurrentPosition() - st->getCurrentPosition();
			Vertex3D now(prev.x * real - prev.y * complex, prev.x * complex + prev.y * real, prev.z);

			for (int i = 1; i < height_granularity; i++)
				getParticle(0, i)->setPosition(now + getParticle(0, i - 1)->getCurrentPosition());
		}
		void rotateZ(float angle){
			Particle *st = getParticle(0, 0);
			Particle *ed = getParticle(0, 1);

			angle = angle * 2 * PI / 360;

			float real = cosf(angle), complex = sinf(angle);
			Vertex3D prev = ed->getCurrentPosition() - st->getCurrentPosition();
			Vertex3D now(prev.x, prev.z * complex + prev.y * real, prev.z * real - prev.y * complex);

			for (int i = 1; i < height_granularity; i++)
				getParticle(0, i)->setPosition(now + getParticle(0, i - 1)->getCurrentPosition());
		}
	};
}

#endif