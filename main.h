#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#define mapW 100 //map size
#define mapH 100

#define enemyCnt 40 //count enemies

#define objectListCount 255

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image/stb_image.h"

#include "camera/camera_module.h"
#include "trees.c"

int width, height;

HWND hwnd;

//Checking whether the coordinates belong to the map
BOOL isCoordInMap(float x, float y)
{
    return (x>=0 && x<mapW && y>=0 && y<mapH);
}

//!-----------------------------------------------------TEXTURE INDEXES
unsigned int texTerain,
    texGrass,
    texFlower,
    texFlower2,
    texMashroom,
    texTree,
    texTree2,
    texBirch;

//!-----------------------------------------------------STRUCTURS
struct {
    float vertex[24];
    GLuint index[36];
}cube ={
        {0,0,0, 0,1,0, 1,1,0, 1,0,0,
         0,0,1, 0,1,1, 1,1,1, 1,0,1},
        {0,1,2, 2,3,0, 4,5,6, 6,7,4,
         3,2,5, 6,7,3, 0,1,5, 5,4,0,
         1,2,6, 6,5,1, 0,3,7, 7,4,0}
};

typedef struct {
    float x,y,z;
    int type;
    float scale;
}TObject;

typedef struct {
    float r,g,b;
}TColor;

typedef struct {
    float x,y,z;
}TCell;

typedef struct{
    float u,v;
}TUV;

typedef struct {
    TObject *obj;
    int objCount;
    int type;
}TWood;

typedef struct {
    int plantsArrayIndex;
    int colorIndex;
}TSelectObject;

//!-----------------------------------------------------PLANTS PRESETS
float cubeWood[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0,
                    0,0,1, 1,0,1, 1,1,1, 0,1,1,
                    0,0,0, 1,0,0, 1,1,0, 0,1,0,
                    0,0,1, 1,0,1, 1,1,1, 0,1,1
};

float cubeUVlog[] =   {0.5,0.5, 1,0.5, 1,0, 0.5,0,
                       0.5,0.5, 1,0.5, 1,0, 0.5,0,
                       0,0.5, 0.5,0.5, 0,0.5, 0.5,0.5,
                       0,0, 0.5,0, 0,0, 0.5,0,
};

float cubeUVleaf[] =   {0,1, 0.5,1, 0.5,0.5, 0,0.5,
                        0,1, 0.5,1, 0.5,0.5, 0,0.5,
                        0,0.5, 0.5,0.5, 0,0.5, 0.5,0.5,
                        0,1, 0.5,1, 0,1, 0.5,1
};

GLuint cubeInd[] = {0,1,2, 2,3,0, 4,5,6, 6,7,4, 8,9,13, 13,12,8,
                    9,10,14, 14,13,9, 10,11,15, 15,14,10, 11,8,12, 12,15,11
};

int cubeIndCount = sizeof(cubeInd)/sizeof(GLuint);

//!-----------------------------------------------------WOODS PRESETS
float object[] = {-0.5,0,0,
                  0.5,0,0,
                  0.5,0,1,
                  -0.5,0,1,
                  0,-0.5,0,
                  0,0.5,0,
                  0,0.5,1,
                  0,-0.5,1};

float objectUV[] = {0,1,
                    1,1,
                    1,0,
                    0,0,
                    0,1,
                    1,1,
                    1,0,
                    0,0};

GLuint objectIndexes[] = {0,1,2,
                          2,3,0,
                          4,5,6,
                          6,7,4};

int objectIndexesCount = sizeof(objectIndexes)/sizeof(GLuint);

//!-----------------------------------------------------PLANTS ARRAYS
TObject *grass = NULL;
int grassSize = 0;

TObject *flowers = NULL;
int flowersSize = 0;

TObject *trees = NULL;
int treesSize = 0;

TWood *woods = NULL;
int woodsSize = 0;

TSelectObject selectArray[objectListCount];
int selectArrayCount = 0;

//sun
float sun[] = {-1,-1,0,
               1,-1,0,
               1,1,0,
               -1,1,0};

//!-----------------------------------------------------MAIN METHODS
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
void playerMove();
void initPlants(int grassCounterSize, int flowersCounterSize);
void creatTrees(TWood *obj, int type);
float mapGetHeight(float x, float y);
void mapCreateHill(int posX, int posY, int radius, int heightExtremum);

#endif // MAIN_H_INCLUDED
