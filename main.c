#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#include "camera/camera_module.h"

// map size
#define mapW 100
#define mapH 100

//count enemies
#define enemyCnt 40

int width, height;

HWND hwnd;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

//Checking whether the coordinates belong to the map
BOOL isCoordInMap(float x, float y)
{
    return (x>=0 && x<mapW && y>=0 && y<mapH);
}

//!-----------------------------------------------------STRUCTURS

struct {
    float vertex[24];
    GLuint index[36];
}cube ={
    {0,0,0, 0,1,0, 1,1,0, 1,0,0, 0,0,1, 0,1,1, 1,1,1, 1,0,1},
    {0,1,2, 2,3,0, 4,5,6, 6,7,4, 3,2,5, 6,7,3, 0,1,5, 5,4,0, 1,2,6, 6,5,1, 0,3,7, 7,4,0}
};

typedef struct {
    float r,g,b;
}TColor;

typedef struct {
    float x,y,z;
}TCell;

//!-----------------------------------------------------Preset

BOOL ShowMask = FALSE;
TCell map[mapW][mapH];
TColor mapColors[mapW][mapH];
GLuint mapIndexes[mapW-1][mapH-1][6];
int mapIndexesCount = sizeof(mapIndexes)/sizeof(GLuint);

//!-----------------------------------------------------MANIPULATOR
void gameMove()
{
    playerMove();
}

float mapGetHeight(float x, float y);
void playerMove()
{
    if (GetForegroundWindow() != hwnd) return;

    float angle = -camera.zRot /180 * M_PI;
    float speed = 0;

    if(GetKeyState('W') <0 && GetKeyState(VK_SHIFT) <0) speed = 0.175;
        else if(GetKeyState('W') <0) speed = 0.1;
    if(GetKeyState('S') <0) speed = -0.1;
    if(GetKeyState('D') <0) {speed = -0.1; angle -= M_PI*0.5;};
    if(GetKeyState('A') <0) {speed = -0.1; angle += M_PI*0.5;};

    if(speed!=0)
    {
        if (!isCoordInMap(camera.x + sin(angle) * speed, camera.y + cos(angle) * speed)) return 0;
        camera.x += sin(angle) * speed;
        camera.y += cos(angle) * speed;
        camera.z = 1.7 + mapGetHeight(camera.x, camera.y);
    }

    float speedCam = 0.2;
    cameraAutoMoveByMouse(400, 300, speedCam);
}

float mapGetHeight(float x, float y)
{
    if(!isCoordInMap(x,y)) return 0;

    int cX = (int)x;
    int cY = (int)y;
        printf("x,y: %f%,%f, cx,cy:%f,%f\n",x,y,cX,cY);

    float h1 = (1-(x-cX))*map[cX][cY].z + (x-cX)*map[cX+1][cY].z;
    float h2 = (1-(x-cX))*map[cX][cY+1].z + (x-cX)*map[cX+1][cY+1].z;
    float h = (1-(y-cY))*h1 + (y-cY)*h2;
            printf("camera[h1,h2,h]: %f%,%f,%f\n",h1, h2, h);

    return h;
}

//!-----------------------------------------------------MAP

void initMap()
{
    for (int i=0; i<mapW; i++){
        int pos = i*mapH;
        for (int j=0; j<mapH; j++)
        {
            if (i < mapW-1 && j < mapH-1)
            {
                mapIndexes[i][j][0] = pos;
                mapIndexes[i][j][1] = pos + 1;
                mapIndexes[i][j][2] = pos + 1 + mapH;

                mapIndexes[i][j][3] = pos + 1 + mapH;
                mapIndexes[i][j][4] = pos + mapH;
                mapIndexes[i][j][5] = pos;
                pos++;
            };

            float dc = (float)(rand() % 20) *0.01;
            mapColors[i][j].r = 0.31 + dc;
            mapColors[i][j].g = 0.5 + dc;
            mapColors[i][j].b = 0.13 + dc;
            map[i][j].x = i;
            map[i][j].y = j;
            map[i][j].z = (rand() %10) *0.05;
        }
    }

    for (int i=0; i<10; i++){
        mapCreateHill(rand()%mapW, rand()%mapH, rand() % mapW/10, mapW/15);
    }

    //pseudo lighted terrain
    for (int i=0; i<mapW; i++){
        for (int j=0; j<mapH; j++){
            float dc = (map[i+1][j+1].z - map[i][j].z)*0.2;
            mapColors[i][j].r += dc;
            mapColors[i][j].g += dc;
            mapColors[i][j].b += dc;
        }
    }
}

//buff the map with hill
void mapCreateHill(int posX, int posY, int radius, int heightExtremum)
{
    if(radius==0 || (posX-radius)<0 || (posY-radius)<0) return;
    for(int i= posX - radius; i <= posX + radius; i++){
        for(int j = posY -radius; j <= posY + radius; j++){
            if(isCoordInMap(i,j)){
                float length = sqrt(pow(posX-i,2) + pow(posY-j,2));
                if(length < radius){
                    length = length / radius * M_PI_2;
                    map[i][j].z =cos(length) * heightExtremum;
                }
            }
        }
    }
}

//!-----------------------------------------------------GAME

void initGame()
{
    glEnable(GL_DEPTH_TEST);
    initMap();
    initEnemys();
    RECT rct;
    GetClientRect(hwnd, &rct);
    windResize(rct.right, rct.bottom);
}

void gameShow()
{
    if(ShowMask) glClearColor(0,0,0,0);
    else glClearColor(0.6, 0.8, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
        applyCamera();

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, map);
        glColorPointer(3, GL_FLOAT, 0, mapColors);
            glPushMatrix();
                if (ShowMask)
                    glColor3f(0,0,0);
                else
                    glColorPointer(3, GL_FLOAT, 0, mapColors);
               glDrawElements(GL_TRIANGLES, mapIndexesCount, GL_UNSIGNED_INT, mapIndexes);
            glPopMatrix();
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        paintEnemys();
    glPopMatrix();
}

//!-----------------------------------------------------ENEMY

struct{
    float x,y,z;
    BOOL active;
}enemies[enemyCnt];

void initEnemys()
{
    for(int i = 0; i<enemyCnt; i++)
    {
        enemies[i].active = TRUE;
        enemies[i].x = rand() % mapW;
        enemies[i].y = rand() % mapH;
        enemies[i].z = rand() % 5;
    }
}

void paintEnemys()
{
    glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, cube.vertex);
        for(int i=0; i<enemyCnt; i++){
            if(!enemies[i].active) continue;
            glPushMatrix();
                glTranslatef(enemies[i].x,enemies[i].y,enemies[i].z);
                if (ShowMask)
                    glColor3ub(255-i, 60, 43);
                else
                    glColor3ub(255, 60, 43);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, cube.index);
            glPopMatrix();
        }
    glDisableClientState(GL_VERTEX_ARRAY);
}

//!-----------------------------------------------------Gameplay

void shooting()
{
    ShowMask = TRUE;
    gameShow();
    ShowMask = FALSE;

    RECT rct;
    GLubyte clr[3];
    GetClientRect(hwnd, &rct);
    glReadPixels(rct.right /2.0, rct.bottom / 2.0, 1, 1,
                        GL_RGB, GL_UNSIGNED_BYTE, clr);
    if (clr[0]>0)
        enemies[255-clr[0]].active = FALSE;
}

//!-----------------------------------------------------MAIN

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          800,
                          600,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    initGame();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */
            gameMove();
            gameShow();
            SwapBuffers(hDC);
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_SIZE:
            width = LOWORD(lParam);
            height = HIWORD(lParam);
            windResize(width, height);
        break;

        case WM_LBUTTONDOWN:
            shooting();
        break;

        case WM_SETCURSOR:
            ShowCursor(FALSE);
        break;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
