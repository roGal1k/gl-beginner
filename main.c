#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#define M_PI 3.1415926535

//screen size
int width, height;

HWND hwnd;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

struct {
    int vertex[24];
    int index[36];
}cube ={{   0,0,0,
            0,1,0,
            1,1,0,
            1,0,0,
            0,0,1,
            0,1,1,
            1,1,1,
            1,0,1},
        {   0,1,2,
            2,3,0,
            4,5,6,
            6,7,4,
            3,2,5,
            6,7,3,
            0,1,5,
            5,4,0,
            1,2,6,
            6,5,1,
            0,3,7,
            7,4,0}};

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

    POINT mouseCur;
    static POINT base= {400, 300};
    GetCursorPos(&mouseCur);
    cameraRotation((base.y - mouseCur.y)/5.0, (base.x - mouseCur.x)/5.0);
    SetCursorPos(base.x, base.y);
}

// map size
#define pW 40
#define pH 40
TCell map[pW][pH];

void initMap(){
    for (int i=0; i<pW; i++){
        for (int j=0; j<pH; j++)
        {
            float dc = (rand() %20) *0.1;
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

    RECT rct;
    GetClientRect(hwnd, &rct);
    WindResize(rct.right, rct.bottom);
}

void gameShow(){
    glClearColor(0.6, 0.8, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
        applyCamera();
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3,GL_FLOAT, 0, cube.vertex);
        for (int i=0; i<pW; i++){
            for (int j=0; j<pH; j++){
                glPushMatrix();
                    glTranslatef(i,j,0);
                    glColor3f(map[i][j].clr.r, map[i][j].clr.g, map[i][j].clr.b);
                    glDrawElements(GL_TRIANGLES,6, GL_UNSIGNED_INT, cube.index);
                glPopMatrix();
            }
        }
    glPopMatrix();
    glDisableClientState(GL_VERTEX_ARRAY);
}

void WindResize(int x, int y){
    glViewport(0, 0, x, y);
    float k = x / (float)y;
    float sz = 0.1;
    glLoadIdentity();
    glFrustum(-k*sz, k*sz, -sz, sz, sz*2, 100);
};

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
            glViewport(0, 0, width, height);
            glLoadIdentity();
            float screenRatio = width/ (float)height;
            glFrustum(-screenRatio, screenRatio, -1,1, -1,1);
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
