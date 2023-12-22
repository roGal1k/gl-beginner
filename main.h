#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

int width, height;

HWND hwnd;

//!-----------------------------------------------------TEXTURES

unsigned int texTerain,
    texGrass,
    texFlower,
    texFlower2,
    texMashroom,
    texTree,
    texTree2;

//!-----------------------------------------------------STRUCTURS

struct {
    float vertex[24];
    GLuint index[36];
}cube ={
    {0,0,0, 0,1,0, 1,1,0, 1,0,0, 0,0,1, 0,1,1, 1,1,1, 1,0,1},
    {0,1,2, 2,3,0, 4,5,6, 6,7,4, 3,2,5, 6,7,3, 0,1,5, 5,4,0, 1,2,6, 6,5,1, 0,3,7, 7,4,0}
};

typedef struct {
    float x,y,z;
    int type;
    float scale;
} TObject;

typedef struct {
    float r,g,b;
}TColor;

typedef struct {
    float x,y,z;
}TCell;

typedef struct{
    float u,v;
}TUV;

//!-----------------------------------------------------PLANTS PRESETS

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

TObject *grass = NULL;
int grassSize = 0;

TObject *flowers = NULL;
int flowersSize = 0;

TObject *trees = NULL;
int treesSize = 0;

#endif // MAIN_H_INCLUDED
