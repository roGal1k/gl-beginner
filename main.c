#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#define M_PI 3.1415926535

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

///! Parametrs screen !///
int width = 1200,
    height = 700;
float koef;

///! Parametrs ball !///
float const GRAVITY_BALL = 0.002;

///! Parametrs players !///
float const GRAVITY_PLAYER = 0.004;

typedef struct{
    float x,y;
    float dx,dy;
    float r;
}TBall;

TBall ball;

TBall players[2];

///! Parametrs net !///
float netHeight = 0.75;


///! Geometry paint !///
void paintRect(float x, float y, float dx, float dy){
    glBegin(GL_TRIANGLE_FAN);
        glVertex2d(x,y);
        glVertex2d(x+dx,y);
        glVertex2d(x+dx,y+dy);
        glVertex2d(x,y+dy);
    glEnd();
}

///! Ball logicks !///
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

BOOL isCross(float x1, float y1, float r, float x2, float y2)
{
    return pow(x1-x2,2)+pow(y1-y2,2) < pow(r,2);
}

void mirrorNet(TBall *obj, float x, float y)
{
    float objVec = atan2(obj->dx, obj->dy);
    float crossVec = atan2(obj->x-x, obj->y-y);

    srand(time(0));
    if(crossVec == 0) crossVec = (rand() % 2 == 0 ? -1 : 1) * 0.023;

    float resVec = M_PI - objVec + crossVec*2;
    float speed = sqrt(pow(obj->dx,2) + pow(obj->dy,2));

    obj->dx = sin(resVec)*speed;
    obj->dy = cos(resVec)*speed;
}

void mirrorBall(TBall *obj, float x, float y, float speed)
{
    float objVec = atan2(obj->dx, obj->dy);
    float crossVec = atan2(obj->x-x, obj->y-y);

    float resVec = speed == 0 ? M_PI - objVec + crossVec*2 : crossVec;
    speed = speed == 0 ? sqrt(pow(obj->dx,2) + pow(obj->dy,2)) : speed;

    obj->dx = sin(resVec)*speed;
    obj->dy = cos(resVec)*speed;
}

void ballMove(TBall *obj){

    obj->x += obj->dx;
    obj->y += obj->dy;

    reflect(&obj->dy, &obj->y, (obj->y < obj->r - 1), obj->r - 1);
    reflect(&obj->dy, &obj->y, (obj->y > 1 - obj->r), 1 - obj->r);

    obj->dy -= GRAVITY_BALL;

    reflect(&obj->dx, &obj->x, (obj->x < obj->r - koef), obj->r - koef);
    reflect(&obj->dx, &obj->x, (obj->x > koef - obj->r), koef - obj->r);

    if (obj->y < -1 + netHeight){
        if (obj->x > 0) reflect(&obj->dx, &obj->x, (obj->x < obj->r), obj->r);
        else reflect(&obj->dx, &obj->x, (obj->x > -obj->r), -obj->r);
    }
    else{
        if(isCross(obj->x, obj->y, obj->r, 0, netHeight - 1)) {
            printf("mirrorTest");
            mirrorNet(obj, 0, netHeight - 1);
        }
    }
}

void initBall(TBall *obj, float x, float y, float dx, float dy, float r){
    obj->x = x;
    obj->y = y;
    obj->dx = dx;
    obj->dy = dy;
    obj->r = r;
}

///! Paint static object !///
void paintBackground(){
    glColor3f(0.83, 0.81, 0.67);
    paintRect(-koef, -1, koef*2, 1);
    glColor3f(0.21, 0.67, 0.88);
    paintRect(-koef, 0, koef*2, 1);
    glColor3f(0.66, 0.85, 1);
    paintRect(-koef, 0.2, koef*2, 1);
}

void paintNet(){
    glColor3f(1, (float)228/255, (float)205/255);
    glLineWidth(6);
    glBegin(GL_LINES);
    glVertex2d(0, -1);
    glVertex2d(0, -1 + netHeight);
    glEnd();
}

///! Player settings !///
void playerMove(TBall *obj, char left, char right, char jump, float wl1, float wl2)
{
    void reflectPlayer(float *da, float *a, BOOL cond, float wall){
        if (!cond) return;
        *da *= 0.85;
        *a = wall;
    }

    static float speed = 0.05;

    reflectPlayer(&obj->dy, &obj->y, (obj->y < obj->r - 1), obj->r - 1);
    reflectPlayer(&obj->dy, &obj->y, (obj->y > 1 - obj->r), 1 - obj->r);

    if(GetKeyState(left)<0) obj->x -= speed;
        else if (GetKeyState(right)<0) obj->x += speed;

    if(obj->x - obj->r < wl1) obj->x = wl1 + obj->r;
    if(obj->x + obj->r > wl2) obj->x = wl2 - obj->r;

    if(GetKeyState(jump) < 0 && (obj -> y < -0.99 + obj->r))
    {
        obj->dy = speed * 1.4;
    }
    obj->y += obj->dy;
    obj->dy -= GRAVITY_PLAYER;
    if(obj->y - obj->r <-1)
    {
        obj->y = -1 + obj->r;
        obj->dy = 0;
    }
    if (isCross(obj->x, obj->y, obj->r+ ball.r, ball.x, ball.y))
    {
        mirrorBall(&ball, obj->x, obj->y, 0.07);
        ball.dy += 0.01;
    }
}

///! Game settings !///
void gameStart(){
    initBall(&ball, 0, 1, 0, 0, 0.15);
    initBall(&players[0], 0.5, 0.1, 0, 0, 0.2);
    initBall(&players[1], -0.5, 0.1, 0, 0, 0.2);
}

void gameShow(){
    paintBackground();
    paintNet();
    glColor3f(0.23,0.29,0.79);
    ballShow(ball);
    glColor3f(0.8,0.1,0.1);
    ballShow(players[0]);
    glColor3f(0.1,0.8,0.1);
    ballShow(players[1]);
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

    koef = (float)width / height;
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
            playerMove(&players[0], 'A', 'D', 'W', 0, koef);
            playerMove(&players[1], VK_LEFT, VK_RIGHT, VK_UP, -koef, 0);

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
