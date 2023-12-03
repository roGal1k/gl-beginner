#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#define M_PI 3.1415926535

//screen size
int width, height;

BOOL ShowMask = FALSE;

HWND hwnd;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

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
    TColor clr;
}TCell;

struct {
    float x,y,z;
    float xRot,zRot;
}camera = {0, 0, 1.7, 70, -40};

void applyCamera(){
    glRotatef(-camera.xRot, 1, 0, 0);
    glRotatef(-camera.zRot, 0, 0, 1);
    glTranslatef(-camera.x, -camera.y, -camera.z);
}

//!-----------------------------------------------------CAMERA

void gameMove()
{
    playerMove();
}

void cameraRotation(float xAngle, float zAngle){
    camera.zRot += zAngle;
    if (camera.zRot < 0) camera.zRot += 360;
    if (camera.zRot >360) camera.zRot -= 360;
    camera.xRot += xAngle;
    if (camera.xRot < 0) camera.zRot = 0;
    if (camera.xRot > 180) camera.zRot = 180;
}

void playerMove(){
    if (GetForegroundWindow() != hwnd) return;

    float angle = -camera.zRot /180 * M_PI;
    float speed = 0;

    if(GetKeyState('W') <0) speed = 0.1;
    if(GetKeyState('S') <0) speed = -0.1;
    if(GetKeyState('D') <0) {speed = -0.1; angle -= M_PI*0.5;};
    if(GetKeyState('A') <0) {speed = -0.1; angle += M_PI*0.5;};

    if(speed!=0)
    {
        camera.x += sin(angle) * speed;
        camera.y += cos(angle) * speed;
    }

    POINT mouseCur;
    static POINT base= {400, 300};
    GetCursorPos(&mouseCur);
    cameraRotation((base.y - mouseCur.y)/5.0, (base.x - mouseCur.x)/5.0);
    SetCursorPos(base.x, base.y);
}

//!-----------------------------------------------------------------GAME

// map size
#define pW 40
#define pH 40
TCell map[pW][pH];

void initMap(){
    for (int i=0; i<pW; i++){
        for (int j=0; j<pH; j++)
        {
            float dc = (rand() %2) *0.1;
            map[i][j].clr.r = 0.31 + dc;
            map[i][j].clr.g = 0.5 + dc;
            map[i][j].clr.b = 0.13 + dc;
        }
    }
}

void WindResize(int x, int y);

void initGame(){
    glEnable(GL_DEPTH_TEST);
    initMap();
    initEnemys();
    RECT rct;
    GetClientRect(hwnd, &rct);
    WindResize(rct.right, rct.bottom);
}

void gameShow(){
    if(ShowMask) glClearColor(0,0,0,0);
    else glClearColor(0.6, 0.8, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
        applyCamera();
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, cube.vertex);
        for (int i=0; i<pW; i++){
            for (int j=0; j<pH; j++){
                glPushMatrix();
                    glTranslatef(i,j,0);
                    if (ShowMask)
                        glColor3f(0,0,0);
                    else
                        glColor3f(map[i][j].clr.r, map[i][j].clr.g, map[i][j].clr.b);
                    glDrawElements(GL_TRIANGLES,6, GL_UNSIGNED_INT, cube.index);
                glPopMatrix();
            }
        }
        glDisableClientState(GL_VERTEX_ARRAY);
        paintEnemys();
    glPopMatrix();
}

void WindResize(int x, int y){
    glViewport(0, 0, x, y);
    float k = x / (float)y;
    float sz = 0.1;
    glLoadIdentity();
    glFrustum(-k*sz, k*sz, -sz, sz, sz*2, 100);
};

//! -------------------------------------------ENEMY

#define enemyCnt 40
struct{
    float x,y,z;
    BOOL active;
}enemys[enemyCnt];

void initEnemys(){
    for(int i = 0; i<enemyCnt; i++)
    {
        enemys[i].active = TRUE;
        enemys[i].x = rand() % pW;
        enemys[i].y = rand() % pH;
        enemys[i].z = rand() % 5;
    }
}

void paintEnemys(){
    glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, cube.vertex);
        for(int i=0; i<enemyCnt; i++){
            if(!enemys[i].active) continue;
            glPushMatrix();
                glTranslatef(enemys[i].x,enemys[i].y,enemys[i].z);
                if (ShowMask)
                    glColor3ub(255-i, 60, 43);
                else
                    glColor3ub(255, 60, 43);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, cube.index);
            glPopMatrix();
        }
    glDisableClientState(GL_VERTEX_ARRAY);
}

void shooting(){
    ShowMask = TRUE;
    gameShow();
    ShowMask = FALSE;

    RECT rct;
    GLubyte clr[3];
    GetClientRect(hwnd, &rct);
    glReadPixels(rct.right /2.0, rct.bottom / 2.0, 1, 1,
                        GL_RGB, GL_UNSIGNED_BYTE, clr);
    if (clr[0]>0)
        enemys[255-clr[0]].active = FALSE;
}


//!--------------------------------------------MAIN

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

            glPushMatrix();
                glBegin(GL_TRIANGLES);
                    glColor3f(0.0f, 0.0f, 0.0f); glVertex2f(0, 0);
                    glColor3f(0.0f, 0.0f, 0.0f); glVertex2f(1, 1);
                    glColor3f(0.0f, 0.0f, 0.0f); glVertex2f(-1, -1);
                glEnd();
            glPopMatrix();

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
            glViewport(0, 0, width, height);
            glLoadIdentity();
            float screenRatio = width/ (float)height;
            float sz = 0.1;
            glFrustum(-screenRatio*sz, screenRatio*sz, -sz, sz, sz*2, 100);
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
