#include <cstdio>
#include <vector>
#include "Vertex3D.h"

#include <GL\glut.h>
#include "Texture.hpp"

namespace ClothSimulation{

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
		void offsetPosition(const Vertex3D v)	{ if (!fixed) currentPosition += v; }
		void setPosition(const Vertex3D v)	{ currentPosition = previousPosition = v; }
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

	class Cloth{
	private:
		int pixmap_id, texture_id;
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
			glColor3f(0.5, 0.5, 0.5);

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
			glEnable(GL_LIGHT1);

			GLfloat lightAmbient1[4] = { 0.7, 0.7, 0.7, 0.0 };
			GLfloat lightPos1[4] = { -1.0, -1.0, 1.0, 0.0 };
			GLfloat lightDiffuse1[4] = { 0.8, 0.8, 0.8, 0.0 };

			glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
			glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient1);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);

			glMaterialfv(GL_FRONT, GL_DIFFUSE, lightDiffuse1);

			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		}

		void renderEnd(){
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHTING);
		}
	public:
		Cloth(float	width, float height, int width_gran, int height_gran, int texture, int pixmap) :
			pixmap_id(pixmap), texture_id(texture),
			width_granularity(width_gran), height_granularity(height_gran){

			particles.resize(width_granularity * height_granularity);
			
			for (int x = 0; x < width_granularity; x++)
				for (int y = 0; y < height_granularity; y++){
					// ÔÚYOZÆ½ÃæÉÏ£¬YÖáÕý·½Ïò£¬zÖá¸º·½Ïò
					Vertex3D pos(height * (y / (float)height_granularity), height * (y / (float)height_granularity), -width * (x / (float)width_granularity));
					
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
				for (int y =0; y < height_granularity - 1; y++){
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
			glColor3f(1, 1, 1);
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
	};
}

ClothSimulation::Cloth banner0(16, 9, 16, 9, TEX_BANNER0, PIXMAP_BANNER0);
ClothSimulation::Cloth banner1(16, 9, 16, 9, TEX_BANNER1, PIXMAP_BANNER1);

void init(){
	glClearColor(1, 1, 1, 0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_COLOR_MATERIAL);
	/*
	glEnable(GL_LIGHT0);
	GLfloat lightPos[4] = { 1.0, 0.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat *)&lightPos);
	*/

	Texture::prepareBanner();
}

void display(){
	//printf("display\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)1280 / (GLfloat)720, 0.1f, 5000.0f);
	gluLookAt(18, 2, 16, 0, 3, -7, 0, 1, 0);
	//gluLookAt(7, 7, 0, 0, 0, 0, 0, 1, 0);
	banner0.addForce(Vertex3D(0, -0.3, 0) * ClothSimulation::STEP_TIMESQUARE); // add gravity each frame, pointing down
	banner0.windForce(Vertex3D(-1.0, 0, -2.0) * ClothSimulation::STEP_TIMESQUARE); // generate some wind each frame
	banner0.stepMove();

	banner1.addForce(Vertex3D(0, -0.3, 0) * ClothSimulation::STEP_TIMESQUARE); // add gravity each frame, pointing down
	banner1.windForce(Vertex3D(-1.0, 0, -2.0) * ClothSimulation::STEP_TIMESQUARE); // generate some wind each frame
	banner1.stepMove();

	/*
	glDisable(GL_LIGHTING); // drawing some smooth shaded background - because I like it ;)
	glBegin(GL_POLYGON);
	glColor3f(0.8f, 0.8f, 1.0f);
	glVertex3f(-200.0f, -100.0f, -100.0f);
	glVertex3f(200.0f, -100.0f, -100.0f);
	glColor3f(0.4f, 0.4f, 0.8f);
	glVertex3f(200.0f, 100.0f, -100.0f);
	glVertex3f(-200.0f, 100.0f, -100.0f);
	glEnd();
	glEnable(GL_LIGHTING);
	*/

	/*

	for (int i = -4; i <= 4; i++)
	for (int j = 0; j >= -15; j--){
	// 0, i, j
	glPushMatrix();
	glTranslated(0, i, j);

	glutSolidSphere(0.1, 30, 30);
	glPopMatrix();
	}
	*/
	glColor3f(0, 1, 0);
	//banner0.drawBallVersion();
	//banner0.drawTriangleVersion();

	//banner0.drawNurbsVersion();

	glPushMatrix();
	glTranslatef(-2, 0, 2);
	glRotatef(10, 0, -1, 0);
	banner1.drawNurbsVersion();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(4.5, 0, -4.5);
	glRotatef(30, 0, -1, 0);
	banner0.drawTriangleVersion();
	//banner1.drawTriangleVersion();
	glPopMatrix();
	
	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);

	glutCreateWindow("Quidditch with waving flag");

	init();
	glutDisplayFunc(display);
	//	glutReshapeFunc(reshape);
	//	glutKeyboardFunc(keyboard);
	//	glutSpecialFunc(arrow_keys);

	glutMainLoop();
}