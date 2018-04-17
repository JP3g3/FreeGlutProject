#include <windows.h>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <time.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/freeglut.h>

#include "glscene.h"
#include "usefull.h"

GLint glScene::winx_ = 0;
GLint glScene::winy_ = 0;

// rozmiary bryły obcinania
const GLfloat left = -50.0f;
const GLfloat right = 50.0f;
const GLfloat bottom = -50.0f;
const GLfloat top = 50.0f;

// położenie kursora myszki
int button_x, button_y;

// wskaźnik naciśnięcia lewego przycisku myszki
int button_state = GLUT_UP;

#define M_PI 3.14159265358979323846

glScene::glScene()
	:
	repaintRequest_(GL_FALSE)
	, alpha_(0)
{
	rot_[0] = rot_[1] = rot_[2] = 0.0f;
}

glScene::~glScene()
{
}

void glScene::Resize()
{
	Resize(winx_, winy_);
}

void glScene::Resize(int _w, int _h)
{
	winx_ = _w;
	winy_ = _h;

	if (_h == 0)
		_h = 1;
	if (_w == 0)
		_w = 1;

	//ustawienie viewportu
	glViewport(0, 0, _w, _h);

	//macierze projekcji
	glMatrixMode(GL_PROJECTION);
	//ustaw aktualna macierz na I
	glLoadIdentity();

	//ustaw uklad wspolrzednych
	//glOrtho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f);
	gluPerspective(75, float(_w) / float(_h), 0.1, 1000.0);

	//macierze modelowania
	glMatrixMode(GL_MODELVIEW);
	//ustaw aktualna macierz na I
	glLoadIdentity();
}

void glScene::SetupRC()
{
	//dodanie tekstur
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_DECAL);
	sun = LoadTexture("sun.bmp");
	grass = LoadTexture("grass.bmp");
	jeans = LoadTexture("jeans.bmp");
	skin = LoadTexture("skin.bmp");
	white = LoadTexture("white.bmp");
	road = LoadTexture("road.bmp");
	sidewalk = LoadTexture("sidewalk.bmp");
	windows = LoadTexture("glass.bmp");

	obj = gluNewQuadric();
	gluQuadricDrawStyle(obj, GLU_FILL);
	gluQuadricNormals(obj, GLU_SMOOTH);/*GLU_FLAT);*/
	gluQuadricOrientation(obj, /*GLU_INSIDE);*/GLU_OUTSIDE);
	gluQuadricTexture(obj, GLU_TRUE);/*GLU_FALSE);*/

	// kolor tła - zawartość bufora koloru
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
}

void glScene::CleanRC()
{
}

void glScene::RenderScene()
{

	//malujemy scene, mozna skasowac flage aby scena ciagle sie nie odmalowywala
	repaintRequest_ = GL_FALSE;

	// czyszczenie bufora koloru i bufora głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	//glPointSize(3.0);

	glEnable(GL_DEPTH_TEST);
	
	ProjectA();

	glPopMatrix();

	glFlush();
}

void glScene::ProjectA() {

	const GLfloat lightCol0[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
	const GLfloat lightPos1[4] = { 0.0f, 50.0f, 0.0f, 0.0f };
	const GLfloat lightPos2[4] = { 0.0f, 60.0f, 0.0f, 1.0f };


	//USTAWIENIE KAMERY
	glTranslatef(0.0f, 0.0f, -15.0f);
	glTranslatef(0.0f, -50.0f, 0.0f);

	glRotatef(rot_[0], 1.0, 0.0f, 0.0f);
	//glRotatef(rot_[1], 0.0f, -50.0f, 0.0f);
	//glRotatef(rot_[2], 0.0f, 0.0f, 1.0f);


	//SLONCE
	glTranslatef(0.0f, 80.0f, 0.0f);
	glCallList(sun);
	gluSphere(obj, 10.0, 40, 40);


	//KSIEZYC
	glTranslatef(0.0f, -150.0f, 0.0f);
	glCallList(white);
	gluSphere(obj, 2.0, 30, 30);
	glTranslatef(0.0f, 70.0f, 0.0f);


	//LAMPY P
	glTranslatef(1.9f, 0.0f, 0.0f);
	DrawStreetLights(0.0, 360.0);
	glTranslatef(-1.9f, 0.0f, 0.0f);
	

	//LAMPY L
	glTranslatef(-1.9f, 0.0f, 0.0f);
	DrawStreetLights(0.0, 360.0);
	glTranslatef(1.9f, 0.0f, 0.0f);


	//SWIATLO
	LightOn(lightCol0, lightPos1);


	//ZIEMIA
	glRotatef(-90.0f, 1.0, 0.0f, 0.0f);
	glCallList(grass);
	gluSphere(obj, 50.0, 60, 60);
		
	
	//ZMIANA POZYCJI SWIATLA
	LightOff();
	LightOn(lightCol0, lightPos2);

	
	//DROGA
	DrawRibbon(road, -1.0, 1.0);


	//PASY
	DrawLine(white);


	//CHODNIK P
	glTranslatef(1.0f, 0.0f, 0.0f);
	DrawRibbon(sidewalk, 0.0, 1.0);
	glTranslatef(-1.0f, 0.0f, 0.0f);


	//CHODNIK L
	glTranslatef(-1.0f, 0.0f, 0.0f);
	DrawRibbon(sidewalk, 0.0, -1.0);
	glTranslatef(1.0f, 0.0f, 0.0f);


	//WIERZOWCE
	for (double i = 0.0; i <= 360.0; i += 10.0) {
		if (((i >= 240.0) && (i <= 260.0)) || ((i >= 330.0) && (i <= 360.0))) {
			glRotatef(-10.0f, 1.0, 0.0f, 0.0f);
			continue;
		}
		GLfloat length = (GLfloat)(4.0 * abs(sin(i)) + 2.0);

		DrawRectangle(sidewalk, 2.0f * length, 1.5f * length, length, 17.0f, 50.001f, 0.0f);
		if ((length >= 2.0f) && (length <= 4.0f)) {
			DrawRectangle(windows, 0.01f, 0.25f * length, 0.25f * length, 17.0f - (2.0f * length), 50.0f + (0.25f * length), -0.3f * length);
			DrawRectangle(windows, 0.01f, 0.25f * length, 0.25f * length, 17.0f - (2.0f * length), 50.0f + (0.25f * length), 0.3f * length);
			DrawRectangle(windows, 0.01f, 0.25f * length, 0.25f * length, 17.0f - (2.0f * length), 50.0f + length, -0.3f * length);
			DrawRectangle(windows, 0.01f, 0.25f * length, 0.25f * length, 17.0f - (2.0f * length), 50.0f + length, 0.3f * length);
		}
		else if ((length > 4.0f) && (length <= 6.0f)) {
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, 17.0f - (2.0f * length), 50.0f + (0.2f * length), -0.5f * length);
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, 17.0f - (2.0f * length), 50.0f + (0.2f * length), 0.5f * length);
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, 17.0f - (2.0f * length), 50.0f + (0.7f * length), -0.5f * length);
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, 17.0f - (2.0f * length), 50.0f + (0.7f * length), 0.5f * length);
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, 17.0f - (2.0f * length), 50.0f + (1.2f * length), -0.5f * length);
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, 17.0f - (2.0f * length), 50.0f + (1.2f * length), 0.5f * length);
		}
		DrawRectangle(sidewalk, 2.0f * length, 1.5f * length, length, -17.0f, 50.001f, 0.0f);
		if ((length >= 2.0f) && (length <= 4.0f)) {
			DrawRectangle(windows, 0.01f, 0.25f * length, 0.25f * length, -17.0f + (2.0f * length), 50.0f + (0.25f * length), -0.3f * length);
			DrawRectangle(windows, 0.01f, 0.25f * length, 0.25f * length, -17.0f + (2.0f * length), 50.0f + (0.25f * length), 0.3f * length);
			DrawRectangle(windows, 0.01f, 0.25f * length, 0.25f * length, -17.0f + (2.0f * length), 50.0f + length, -0.3f * length);
			DrawRectangle(windows, 0.01f, 0.25f * length, 0.25f * length, -17.0f + (2.0f * length), 50.0f + length, 0.3f * length);
		}
		else if ((length > 4.0f) && (length <= 6.0f)) {
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, -17.0f + (2.0f * length), 50.0f + (0.2f * length), -0.5f * length);
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, -17.0f + (2.0f * length), 50.0f + (0.2f * length), 0.5f * length);
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, -17.0f + (2.0f * length), 50.0f + (0.7f * length), -0.5f * length);
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, -17.0f + (2.0f * length), 50.0f + (0.7f * length), 0.5f * length);
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, -17.0f + (2.0f * length), 50.0f + (1.2f * length), -0.5f * length);
			DrawRectangle(windows, 0.01f, 0.2f * length, 0.2f * length, -17.0f + (2.0f * length), 50.0f + (1.2f * length), 0.5f * length);
		}
		glRotatef(-10.0f, 1.0, 0.0f, 0.0f);
	}


	glPopMatrix();
	glPushMatrix();


	//USTAWIENIE KAMERY
	glTranslatef(0.0f, 0.0f, -15.0f);
	glTranslatef(0.0f, -50.0f, 0.0f);
	glRotatef(14.0f, 1.0, 0.0f, 0.0f);


	//GIGANT
	DrawRectangle(jeans, 0.1f, 0.3f, 0.1f, -0.15f, 50.3f, 0.0f);
	DrawRectangle(jeans, 0.1f, 0.3f, 0.1f, 0.15f, 50.3f, 0.0f);
	
	DrawRectangle(grass, 0.3f, 0.3f, 0.1f, 0.0f, 50.9f, 0.0f);
	
	DrawRectangle(skin, 0.05f, 0.2f, 0.05f, 0.35f, 50.95f, 0.0f);
	DrawRectangle(skin, 0.05f, 0.2f, 0.05f, -0.35f, 50.95f, 0.0f);
	DrawRectangle(skin, 0.1f, 0.1f, 0.1f, 0.0f, 51.3f, 0.0f);
	
	DrawRectangle(sun, 0.1f, 0.02f, 0.1f, 0.0f, 51.42f, 0.0f);


	//WYLACZANIE SWIATLA
	LightOff();

}

void glScene::LightOn(const GLfloat * LightColor, const GLfloat * LightPosition) {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
}

void glScene::LightOff() {
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
}

void glScene::DrawStreetLights(double FStep, double LStep) {
	glPushMatrix();
	for (double i = FStep; i <= LStep; i += 10.0) {

		glPushMatrix();
		glCallList(road);
		glTranslatef(0.0f, 50.01f, 0.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(obj, 0.025, 0.025, 1.5, 10, 10);
		glPopMatrix();

		glPushMatrix();
		glCallList(white);
		glTranslatef(0.0f, 51.52f, 0.0f);
		gluSphere(obj, 0.1, 10, 10);
		glPopMatrix();

		glRotatef(-10.0f, 1.0, 0.0f, 0.0f);
	}
	glPopMatrix();
}

void glScene::DrawLine(GLint T, double r, double LoopStep)
{
	glCallList(T);
	GLfloat x = 0.0f, y = 0.0f, z = 0.0f;
	glBegin(GL_LINES);
	for (double i = 0.0; i <= 360.0; i += LoopStep) {
		x = (GLfloat)(cos(i * M_PI / 180.0) * r);
		y = (GLfloat)(sin(i * M_PI / 180.0) * r);
		glTexCoord3f(z, x, y);
		glVertex3f(z, x, y);
	}
	glEnd();
}

void glScene::DrawRibbon(GLint T, GLfloat FStep, GLfloat SStep, double r, double LoopStep)
{
	glCallList(T);
	GLfloat x = 0.0f, y = 0.0f, z = 0.0f;
	glBegin(GL_TRIANGLE_STRIP);
	for (double i = 0.0; i <= 360.0; i += LoopStep) {
		x = (GLfloat)(cos(i * M_PI / 180.0) * r);
		y = (GLfloat)(sin(i * M_PI / 180.0) * r);
		glTexCoord3f((z + FStep), x, y);
		glVertex3f((z + FStep), x, y);
		glTexCoord3f((z + SStep), x, y);
		glVertex3f((z + SStep), x, y);
	}
	glEnd();
}

void glScene::DrawRectangle(GLint T, float w, float h, float l, float posX, float posY, float posZ)
{
	glPushMatrix();
	glCallList(T);
	glTranslatef(posX, posY, posZ);

	glBegin(GL_QUADS);

	glTexCoord3f(w, h, -l);
	glTexCoord3f(-w, h, -l);
	glTexCoord3f(-w, h, l);
	glTexCoord3f(w, h, l);

	glVertex3f(w, h, -l);
	glVertex3f(-w, h, -l);
	glVertex3f(-w, h, l);
	glVertex3f(w, h, l);

	glTexCoord3f(w, -h, l);
	glTexCoord3f(-w, -h, l);
	glTexCoord3f(-w, -h, -l);
	glTexCoord3f(w, -h, -l);

	glVertex3f(w, -h, l);
	glVertex3f(-w, -h, l);
	glVertex3f(-w, -h, -l);
	glVertex3f(w, -h, -l);

	glTexCoord3f(w, h, l);
	glTexCoord3f(-w, h, l);
	glTexCoord3f(-w, -h, l);
	glTexCoord3f(w, -h, l);

	glVertex3f(w, h, l);
	glVertex3f(-w, h, l);
	glVertex3f(-w, -h, l);
	glVertex3f(w, -h, l);

	glTexCoord3f(w, -h, -l);
	glTexCoord3f(-w, -h, -l);
	glTexCoord3f(-w, h, -l);
	glTexCoord3f(w, h, -l);

	glVertex3f(w, -h, -l);
	glVertex3f(-w, -h, -l);
	glVertex3f(-w, h, -l);
	glVertex3f(w, h, -l);

	glTexCoord3f(-w, h, l);
	glTexCoord3f(-w, h, -l);
	glTexCoord3f(-w, -h, -l);
	glTexCoord3f(-w, -h, l);
	
	glVertex3f(-w, h, l);
	glVertex3f(-w, h, -l);
	glVertex3f(-w, -h, -l);
	glVertex3f(-w, -h, l);

	glTexCoord3f(w, h, -l);
	glTexCoord3f(w, h, l);
	glTexCoord3f(w, -h, l);
	glTexCoord3f(w, -h, -l);

	glVertex3f(w, h, -l);
	glVertex3f(w, h, l);
	glVertex3f(w, -h, l);
	glVertex3f(w, -h, -l);

	glEnd();
	glPopMatrix();
};

void glScene::HairGenerate(float x, float y, float z)
{
	glVertex3f(x + (x / 2), y + (y / 2), z + (z / 2));
	glVertex3f(x, y, z);
}

void glScene::NextStripe(GLfloat beginPos, GLfloat endPos, GLfloat yValue, GLfloat zValue, GLfloat step)
{
	glBegin(GL_TRIANGLE_STRIP);

	for (float i = beginPos; i <= endPos; i += step)
	{
		glColor3f(beginPos, endPos, beginPos*endPos);

		glVertex3f(i, yValue, zValue);
		glVertex3f(i, yValue - 0.2f, zValue);
	}
	glEnd();
}

void glScene::KeyboardFunction(char _key, int, int)
{
	if (_key == 32)
	{
		rot_[0] = rot_[1] = rot_[2] = 0.0;
		Resize(winx_, winy_);
	}
	repaintRequest_ = GL_TRUE;
}

void glScene::KeyboardFunction(int _key, int, int)
{
	
	if (_key == GLUT_KEY_UP)
		rot_[0] += 1.0;
	else if (_key == GLUT_KEY_DOWN)
		rot_[0] -= 0.5;
	else if (_key == GLUT_KEY_RIGHT)
		rot_[1] += 1.0;
	else if (_key == GLUT_KEY_LEFT)
		rot_[1] -= 0.5;
	else if (_key == GLUT_KEY_PAGE_UP)
		rot_[2] += 0.5;
	else if (_key == GLUT_KEY_PAGE_DOWN)
		rot_[2] -= 0.5;

	if (rot_[0] > 360) rot_[0] = 360 - rot_[0];
	if (rot_[1] > 360) rot_[1] = 360 - rot_[1];
	if (rot_[2] > 360) rot_[2] = 360 - rot_[2];

	repaintRequest_ = GL_TRUE;
}

void glScene::MouseMove(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rot_[1] += (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rot_[0] -= (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

void glScene::MouseLBDown(int x, int y)
{
	button_state = GLUT_DOWN;
	button_x = x;
	button_y = y;
	
}

void glScene::MouseLBUp(int, int)
{
	button_state = GLUT_UP;
}

void glScene::Timer()
{
	repaintRequest_ = GL_TRUE;
	rot_[0] += 0;
	alpha_ += 1;
}

bool glScene::WantTimer()
{
	return  true;
}

int glScene::GetTimer()
{
	return 50;
}


