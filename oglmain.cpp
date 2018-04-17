#include <stdio.h>

#include <GL/freeglut.h>


#include "glscene.h"




int key_;
int winy_;

//globalny objekt ze scena
//tam w srodku jest OPENGL
glScene scene_;


void SampleKeyboard( unsigned char _cChar, int _nMouseX, int _nMouseY );
void SpecialKeyboard( int _cChar, int _nMouseX, int _nMouseY );
void MouseFunc( int _button, int _state, int _x, int _y);
void MouseMove( int _x, int _y );
void Idle(void);
void Reshape(int _x, int _y);
void Redraw(void);
void Timer( int _value);



int main(int argc, char* argv[])
{
	int nWindow;

	printf("FreeGLUT project\n");

    glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	//glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE /*| GLUT_BORDERLESS*/); // do try as well with GLUT_BORDERLESS and GLUT_CAPTIONLESS
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    /* The window position you request is the outer top-left of the window,
     * the client area is at a different position if the window has borders
     * and/or a title bar.
     */
    glutInitWindowPosition(150,250);
    glutInitWindowSize(1000,400);

    nWindow = glutCreateWindow("okno FreeGLUT");
	printf("Okno utworzone, identyfikator: %d", nWindow);

	scene_.SetupRC();
	if(scene_.WantTimer())
	{
		glutTimerFunc(scene_.GetTimer(), Timer, 1);
	}

    glutKeyboardFunc( SampleKeyboard );
    glutSpecialFunc( SpecialKeyboard );
	glutMouseFunc( MouseFunc );
	glutMotionFunc( MouseMove );
	glutPassiveMotionFunc( MouseMove );
    glutDisplayFunc( Redraw );
    glutReshapeFunc( Reshape );
	glutIdleFunc( Idle );

    glutMainLoop();

	scene_.CleanRC();

	return 0;
}

void SampleKeyboard( unsigned char _cChar, int _nMouseX, int _nMouseY )
{
	scene_.KeyboardFunction(_cChar, _nMouseX, _nMouseY);
}

void SpecialKeyboard( int _cChar, int _nMouseX, int _nMouseY )
{
	scene_.KeyboardFunction(_cChar, _nMouseX, _nMouseY);
}

void MouseFunc( int _button, int _state, int _x, int _y)
{
	if(_button == GLUT_LEFT_BUTTON && _state == GLUT_UP)
		scene_.MouseLBUp( _x, _y);
	else if(_button == GLUT_LEFT_BUTTON && _state == GLUT_DOWN)
		scene_.MouseLBDown( _x, _y);
}

void MouseMove( int _x, int _y )
{
	scene_.MouseMove(_x, _y);
}

void Idle(void)
{
	if(scene_.NeedRefresh())
		glutPostRedisplay();
}
void Reshape(int , int )
{
	scene_.Resize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void Redraw(void)
{
	scene_.RenderScene();
	glutSwapBuffers();
}

void Timer(int _v)
{
	if(_v == 1)
	{
		scene_.Timer();
		glutTimerFunc(scene_.GetTimer(), Timer, 1);
	}
}