#include <cstdio>
#include <vector>
#include "Vertex3D.h"

#include <GL\glut.h>

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
				//printf("move!\n");
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

		void drawTriangle(Particle *p1, Particle *p2, Particle *p3, const Vertex3D color)
		{
			glColor3fv((GLfloat*)&color);

			glNormal3fv((GLfloat *)&(p1->getNormal().normalize()));
			glVertex3fv((GLfloat *)&(p1->getCurrentPosition()));

			glNormal3fv((GLfloat *)&(p2->getNormal().normalize()));
			glVertex3fv((GLfloat *)&(p2->getCurrentPosition()));

			glNormal3fv((GLfloat *)&(p3->getNormal().normalize()));
			glVertex3fv((GLfloat *)&(p3->getCurrentPosition()));
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
	public:
		Cloth(float	width, float height, int width_gran, int height_gran) :
			width_granularity(width_gran), height_granularity(height_gran){

			particles.resize(width_granularity * height_granularity);
			for (int x = 0; x < width_granularity; x++)
				for (int y = 0; y < height_granularity; y++){
					// ÔÚYOZÆ½ÃæÉÏ£¬YÖáÕý·½Ïò£¬zÖá¸º·½Ïò
					Vertex3D pos(height * (y / (float)height_granularity), height * (y / (float)height_granularity), -width * (x / (float)width_granularity));
					particles[y * width_granularity + x] = Particle(pos, PARTICLE_MASS);
					if (x == 0)
						particles[y * width_granularity + x].setFixed();
				}

			bindAdjacentConstraints();
		}


		void drawBallVersion(){
			for (int i = 0; i < particles.size(); i++){
				Vertex3D pos(particles[i].getCurrentPosition());

				glPushMatrix();
				glTranslated(pos.x, pos.y, pos.z);

				glutSolidSphere(0.1, 30, 30);
				glPopMatrix();
			}
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

ClothSimulation::Cloth banner(16, 9, 16, 9);

void init(){
	//  ÑÕÉ«½¥±ä
	glShadeModel(GL_SMOOTH);

	glClearColor(1, 1, 1, 0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	/*
	glEnable(GL_LIGHT0);
	GLfloat lightPos[4] = { 1.0, 0.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat *)&lightPos);
	*/

	glEnable(GL_LIGHT1);

	GLfloat lightAmbient1[4] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat lightPos1[4] = { 1.0, 1.0, 0.0, 0.0 };
	GLfloat lightDiffuse1[4] = { 0.8, 0.8, 0.8, 0.0 };

	glLightfv(GL_LIGHT1, GL_POSITION, (GLfloat *)&lightPos1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, (GLfloat *)&lightAmbient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, (GLfloat *)&lightDiffuse1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void display(){
	//printf("display\n");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)1280 / (GLfloat)720, 0.1f, 5000.0f);
	gluLookAt(18, 2, 10, 0, 2, -7, 0, 1, 0);

	banner.addForce(Vertex3D(0, -0.3, 0) * ClothSimulation::STEP_TIMESQUARE); // add gravity each frame, pointing down
	//banner.windForce(Vertex3D(-0.4, 0, -0.8) * ClothSimulation::STEP_TIMESQUARE); // generate some wind each frame
	banner.stepMove();


	//gluLookAt(5, 5, 5, 0, 0, 0, 0, 1, 0);
	//gluLookAt(1, 1, 1, 0, 0, 0, 0, 1, 0);
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
	banner.drawBallVersion();

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