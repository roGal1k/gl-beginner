#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#define M_PI 3.1415926535

float vertexRectangle[] = {1,1,0,
                        1,-1,0,
                       -1,-1,0,
                       -1,1,0};

float vertexPyramid[] = {0,0,2,
                         1,1,0,
                         1,-1,0,
                        -1,-1,0,
                        -1,1,0,
                         1,1,0};

POINTFLOAT posPlayer = {0,0};

float xAlpha = 20;
float yAlpha = 0;

void ShowWorld()
{
    glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT,0, &vertexRectangle);
        for (int i=0; i<20; i++)
        {
            for (int j=0; j<20; j++)
            {
                glPushMatrix();
                    if((i+j)%2 == 0) glColor3f(0,0.5,0);
                    else glColor3f(0.9,0.9,0.9);
                    glTranslatef(i*2, j*2, 0);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glPopMatrix();
            }
        }
    glDisableClientState(GL_VERTEX_ARRAY);

}

void PaintCube(){
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT,0, &vertexRectangle);
    glTranslatef(10, 10, 0.001);

    //bottom
    glPushMatrix();
        glColor3f(0.2,0.2,0.2);
        glTranslatef(0, 0, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glPopMatrix();

    //top
    glPushMatrix();
        glColor3f(0.2,0.2,0.2);
        glTranslatef(0, 0, 2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glPopMatrix();

    //back gorizontal
    glPushMatrix();
        glColor3f(0.2,0.2,0.2);
        glTranslatef(0, 1, 1);
        glRotatef(90,1,0,0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glPopMatrix();

    //front gorizontal
    glPushMatrix();
        glColor3f(0.2,0.2,0.2);
        glTranslatef(0, -1, 1);
        glRotatef(90,1,0,0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glPopMatrix();

    //left
    glPushMatrix();
        glColor3f(0.2,0.2,0.2);
        glTranslatef(1, 0, 1);
        glRotatef(90,1,0,0);
        glRotatef(90,0,1,0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glPopMatrix();

    //right
    glPushMatrix();
        glColor3f(0.2,0.2,0.2);
        glTranslatef(-1, 0, 1);
        glRotatef(90,1,0,0);
        glRotatef(90,0,1,0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glPopMatrix();
}

void PaintPyramid(){

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT,0, &vertexPyramid);
    glTranslatef(-5, 25, 0.001);

    //bottom
    glPushMatrix();
        glColor3f(0.5,0.4,0.1);
        glTranslatef(0, 0, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    glPopMatrix();
}

void MoveCamera(){
    if(GetKeyState(VK_UP) <0) xAlpha =++xAlpha > 180 ? 180 : xAlpha;
    if(GetKeyState(VK_DOWN) <0) xAlpha =--xAlpha <0 ? 0 : xAlpha;

    BOOL cam=FALSE;
    if(GetKeyState(VK_LEFT) <0) yAlpha++;
    if(GetKeyState(VK_RIGHT) <0) yAlpha--;

    float angle = -yAlpha /180 * M_PI;
    float speed = 0;

    if(GetKeyState('W') <0) speed = 0.1;
    if(GetKeyState('S') <0) speed = -0.1;
    if(GetKeyState('D') <0) {speed = -0.1; angle -= M_PI*0.5;};
    if(GetKeyState('A') <0) {speed = -0.1; angle += M_PI*0.5;};

    if(speed!=0)
    {
        posPlayer.x += sin(angle) * speed;
        posPlayer.y += cos(angle) * speed;
    }

    glRotatef(-xAlpha,1,0,0);
    glRotatef(-yAlpha,0,0,1);
    glTranslatef(-posPlayer.x, -posPlayer.y,-3);
}

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

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
                          1000,
                          1000,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    glEnable(GL_DEPTH_TEST);

    glLoadIdentity();
    glFrustum(-1,1, -1,1, 2,100);

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
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
                MoveCamera();
                ShowWorld();
                PaintCube();
                PaintPyramid();
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
