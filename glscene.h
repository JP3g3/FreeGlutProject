#if !defined(_glscene_h)
#define _glscene_h
#include "usefull.h"
#include <GL/freeglut.h>

class glScene
{
	public:
   	glScene();
      virtual ~glScene();

      //wywolywana gdy rozmiar okna na ekranie ulega zmianie
      virtual void Resize(int, int);
      virtual void Resize();

		//wolana gdy zostanie wcisniety klawisz z klawiatury
      //Kody klawiszy VK_ z naglowka windows.h
      virtual void KeyboardFunction(char, int, int);
      virtual void KeyboardFunction(int, int, int);

      virtual void MouseMove(int, int);
	  virtual void MouseLBDown(int, int);
	  virtual void MouseLBUp(int, int);

      //wywolywana po utworzeniu klasy sceny. Ma za zadanie inicjalizacje OpenGL
      virtual void SetupRC();

      //wolana przed zniszczeniem objektu sceny. Ma za zadanie przywrocic
      //stan poczatkowy OpenGL
      virtual void CleanRC();

      //Wolana przez Timer.
      virtual void Timer();

      //ma zwrocic prawde jesli nasza scena wymaga timera
      virtual bool WantTimer();

      //ma zwrocic liczbe milisekund co ile ma byc wolana metoda Timer()
      virtual int GetTimer();

	  //zwraca stan flagi repaintRequest_ - jesli prawda to OGLmain powinien odmalowac okno
	  virtual bool NeedRefresh();

      virtual void RenderScene();

	  virtual void ProjectA();
	  virtual void LightOn(const GLfloat * LightColor, const GLfloat * LightPosition);
	  virtual void LightOff();
	  virtual void DrawStreetLights(double FStep, double LStep);
	  virtual void DrawLine(GLint T, double r = 50.05, double LoopStep = 2.5);
	  virtual void DrawRibbon(GLint T, GLfloat FStep = 0.0f, GLfloat SStep = 0.0f, double r = 50.001, double LoopStep = 1.0);
	  virtual void DrawRectangle(GLint T, float w, float h, float l, float PosX = 0.0f, float posY = 0.0f, float posZ = 0.0f);
	  virtual void HairGenerate(float x, float y, float z);
	  virtual void NextStripe(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);

   protected:
		GLfloat rot_[3];
		GLfloat alpha_;
      static GLint winx_, winy_;
	  GLboolean repaintRequest_;

	  GLuint sun, grass, jeans, skin, white, road, sidewalk, windows, brown;
	  GLUquadric* obj;
};

inline
bool glScene::NeedRefresh()
{
	return (bool)(repaintRequest_ == GL_TRUE ? true : false);
}


#endif
