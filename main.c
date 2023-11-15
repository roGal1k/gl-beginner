#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#define M_PI 3.1415926535

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

int width = 1200,
    height = 700;

float koef;

float const GRAVITY_BALL = 0.002;

typedef struct{
    float x,y;
    float dx,dy;
    float r;
}TBall;

TBall ball;

void reflect(float *da, float *a, BOOL cond, float wall){
    if (!cond) return;
    *da *= -0.85;
    *a = wall;
}

void ballShow(TBall obj){
    glPushMatrix();
        glTranslatef(obj.x, obj.y, 0);
        glScalef(obj.r, obj.r, 1);
        paintCircle(20);
    glPopMatrix();
}

void ballMove(TBall *obj){
    obj->x += obj->dx;
    obj->y += obj->dy;

    reflect(&obj->dy, &obj->y, (obj->y < obj->r - 1), obj->r - 1);
    reflect(&obj->dy, &obj->y, (obj->y > 1 - obj->r), 1 - obj->r);

    obj->dy -= GRAVITY_BALL;

    reflect(&obj->dx, &obj->x, (obj->x < obj->r - koef), obj->r - koef);
    reflect(&obj->dx, &obj->x, (obj->x < koef - obj->r), koef - obj->r);
}

void initBall(TBall *obj, float x, float y, float dx, float dy, float r){
    obj->x = x;
    obj->y = y;
    obj->dx = dx;
    obj->dy = dy;
    obj->r = r;
}

void gameStart(){
    initBall(&ball, 0, 0, 0.05, -0.01, 0.1);
}

void paintCircle(int size){
    float x,y;
    float da=M_PI*2.0/size;
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        for (int i=0; i<=size; i++)
        {
            x = sin(da * i);
            y = cos(da * i);
            glVertex2f(x,y);
        }
    glEnd();
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
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
                          width,
                          height,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    koef = (float)width/height;
    glScalef(1/koef, 1, 1);

    gameStart();

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
            glClearColor(0.0f, 0.2f, 0.4f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ballMove(&ball);
            glColor3f(1,1,1);
            ballShow(ball);

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
