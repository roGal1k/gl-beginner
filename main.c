#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#define M_PI 3.1415926535

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


void paintHouse(float  x, float y, float size){

    glLineWidth(0.25f);

    glBegin(GL_TRIANGLES);

    //build
        glColor3f(0.4f, 0.4f, 0.2f); glVertex2d(x, y);
        glColor3f(0.4f, 0.4f, 0.2f); glVertex2d(x, y+size);
        glColor3f(0.4f, 0.4f, 0.2f); glVertex2d(x+size, y+size);

        glColor3f(0.4f, 0.4f, 0.2f); glVertex2d(x+size, y+size);
        glColor3f(0.4f, 0.4f, 0.2f); glVertex2d(x, y);
        glColor3f(0.4f, 0.4f, 0.2f); glVertex2d(x+size, y);

    //roof
        glColor3f(0.4f, 0.5f, 0.2f); glVertex2d(x+size, y+size);
        glColor3f(0.4f, 0.5f, 0.2f); glVertex2d((x+size/2.0f), y+size*1.5f);
        glColor3f(0.4f, 0.5f, 0.2f); glVertex2d(x, y+size);

    //window
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2d((x+size/2.0f)-size/4.0f, (y+size/2.0f)-size/4.0f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2d((x+size/2.0f)-size/4.0f, (y+size/2.0f)+size/4.0f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2d((x+size/2.0f)+size/4.0f, (y+size/2.0f)+size/4.0f);

        glColor3f(0.0f, 0.0f, 1.0f); glVertex2d((x+size/2.0f)+size/4.0f, (y+size/2.0f)-size/4.0f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2d((x+size/2.0f)-size/4.0f, (y+size/2.0f)-size/4.0f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2d((x+size/2.0f)+size/4.0f, (y+size/2.0f)+size/4.0f);

    glEnd();

    glBegin(GL_LINES);

    // roof triangle border
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d(x+size, y+size);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f), y+size*1.5f);

        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d(x, y+size);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f), y+size*1.5f);

    // body triangle border
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d(x, y+size);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d(x+size, y+size);

        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d(x, y);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d(x, y+size);

        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d(x+size, y);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d(x+size, y+size);

        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d(x, y);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d(x+size, y);

    // window border
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f)+size/4.0f, (y+size/2.0f)+size/4.0f);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f)+size/4.0f, (y+size/2.0f)-size/4.0f);

        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f)-size/4.0f, (y+size/2.0f)+size/4.0f);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f)-size/4.0f, (y+size/2.0f)-size/4.0f);

        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f)-size/4.0f, (y+size/2.0f)+size/4.0f);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f)+size/4.0f, (y+size/2.0f)+size/4.0f);

        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f)-size/4.0f, (y+size/2.0f)-size/4.0f);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f)+size/4.0f, (y+size/2.0f)-size/4.0f);

        //central border in window
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f)-size/4.0f, (y+size/2.0f));
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f)+size/4.0f, (y+size/2.0f));

        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f), (y+size/2.0f)-size/4.0f);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d((x+size/2.0f), (y+size/2.0f)+size/4.0f);

    glEnd();

}

void paintTriangle(){
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f); glVertex2d(0.0f, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex2d(0.5f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2d(0.0f, 0.5f);
    glEnd();
}

void paintTrees(){

    glBegin(GL_TRIANGLES);
        // body tree
        glColor3f(0.65f, 0.2f, 0.0f); glVertex2d(0.8f, -0.4f);
        glColor3f(0.65f, 0.2f, 0.0f); glVertex2d(0.8f, 0.2f);
        glColor3f(0.65f, 0.2f, 0.0f); glVertex2d(0.9f, 0.2f);

        glColor3f(0.65f, 0.2f, 0.0f); glVertex2d(0.9f, -0.4f);
        glColor3f(0.65f, 0.2f, 0.0f); glVertex2d(0.9f, 0.2f);
        glColor3f(0.65f, 0.2f, 0.0f); glVertex2d(0.8f, -0.4f);
    glEnd();

    float cnt = 30;
    float r2 = 0.1;
    float a = M_PI * 2 / cnt;

    glBegin(GL_TRIANGLE_FAN);

        r2 = 0.3;
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2d(0.85, 0.35);
        for (int i=-1; i<cnt; i++)
        {
            float x = 0.85 + (sin(a * i) * r2);
            float y = 0.35 + (cos(a * i) * r2);
            glVertex2d(x,y);
        }

        r2 = 0.12;
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2d(0.65, 0.15);
        for (int i=-1; i<cnt; i++)
        {
            float x = 0.65 + (sin(a * i) * r2);
            float y = 0.15 + (cos(a * i) * r2);
            glVertex2d(x,y);
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
                          500,
                          500,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

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

            glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            /*
            glLoadIdentity();
            glScalef(0.3, 0.3, 1);
            glRotatef(0.5,0,0,1);

            for (int i=0; i<8; i++)
            {
                glPushMatrix();
                glRotatef(45*i,0,0,1);
                glTranslatef(0.5f, 0.0f, 0.0f);
                paintTriangle();
                glPopMatrix();
            }
            */

            glPushMatrix();

            glBegin(GL_TRIANGLES);
                glColor3f(0.0f, 0.1f, 0.6f); glVertex2d(-1.0f, 1.0f);
                glColor3f(0.0f, 0.1f, 0.6f); glVertex2d(-1.0f, 0.0f);
                glColor3f(0.0f, 0.1f, 0.6f); glVertex2d(1.0f, 1.0f);

                glColor3f(0.0f, 0.1f, 0.6f); glVertex2d(1.0f, 1.0f);
                glColor3f(0.0f, 0.1f, 0.6f); glVertex2d(1.0f, 0.0f);
                glColor3f(0.0f, 0.1f, 0.6f); glVertex2d(-1.0f, 0.0f);
            glEnd();

            paintHouse(-0.5, -0.4, 0.4);
            paintHouse(0.1, -0.1, 0.4);
            paintHouse(0.5, -0.7, 0.5);
            paintHouse(0.1, -0.9, 0.3);
            paintHouse(-0.9, -0.9, 0.4);

            float cnt = 30;
            float r2 = 0.1;
            float a = M_PI * 2 / cnt;

            // sun
            glBegin(GL_TRIANGLE_FAN);

                glColor3f(1.0f, 1.0f, 0.0f);
                glVertex2d(0.8, 0.85);

                for (int i=-1; i<cnt; i++)
                {
                    float x = 0.8 + (sin(a * i) * r2);
                    float y = 0.8 + (cos(a * i) * r2);
                    glVertex2d(x,y);
                }

            glEnd();

            glPopMatrix();

            SwapBuffers(hDC);

            theta += 1.0f;
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

