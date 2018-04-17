#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>
#include <stdarg.h>
#include <stdio.h>

#include <GL/freeglut.h>

#include "glscene.h"

#pragma once
void SaveScene();
void* ReadDIBitmap(BITMAPINFO**);
void* LoadDIBitmap(char*, BITMAPINFO**);
int SaveDIBitmap(char*, BITMAPINFO*, void*);
GLuint LoadTexture(char* fileName);
