#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#define M_PI 3.1415926535

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

typedef struct {
    POINTFLOAT peaks[7];
    float colors[18];
    GLuint indexs[6];
    void (*initPine)();
    void (*paintPineWithUseVertexArray)();
    void (*paintPineWithUseIndexArray)();
}TPine;

TPine pine;

void initPine() {
    pine.peaks[0].x = 0.0;
    pine.peaks[0].y = 1;

    pine.peaks[1].x = 0.5;
    pine.peaks[1].y = 0.5;
    pine.peaks[2].x = -0.5;
    pine.peaks[2].y = 0.5;

    pine.peaks[3].x = 0;
    pine.peaks[3].y = 0.5;

    pine.peaks[4].x = -0.7;
    pine.peaks[4].y = -0.2;


    pine.peaks[5].x = 0.7;
    pine.peaks[5].y = -0.2;

    pine.peaks[6].x = 0.7;
    pine.peaks[6].y = -0.2;


    pine.colors[0] = 1;  pine.colors[1] = 0;  pine.colors[2] = 0;
    pine.colors[3] = 0;  pine.colors[4] = 1;  pine.colors[5] = 0;
    pine.colors[6] = 0;  pine.colors[7] = 1;  pine.colors[8] = 0;
    pine.colors[9] = 0;  pine.colors[10] = 0; pine.colors[11] = 1;
    pine.colors[12] = 0; pine.colors[13] = 0; pine.colors[14] = 1;
    pine.colors[15] = 1; pine.colors[16] = 1; pine.colors[17] = 0;

    pine.indexs[0] = 0; pine.indexs[1] = 1; pine.indexs[2] = 1;
    pine.indexs[3] = 4; pine.indexs[4] = 4; pine.indexs[5] = 5;
}

void paintPineWithUseVertexArray() {
    glVertexPointer(2, GL_FLOAT, 0, pine.peaks);
    glEnableClientState(GL_VERTEX_ARRAY);

    //glColorPointer(3, GL_FLOAT, 0, pine.colors);
    //glEnableClientState(GL_COLOR_ARRAY);

    glColor3f(0,2,0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 7);

    glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_COLOR_ARRAY);
}

void paintPineWithUseIndexArray() {
    glVertexPointer(2, GL_FLOAT, 0, pine.peaks);
    glEnableClientState(GL_VERTEX_ARRAY);

    glColorPointer(3, GL_FLOAT, 0, pine.colors);
    glEnableClientState(GL_COLOR_ARRAY);

    glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, pine.indexs);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void paintMainTask(){
    POINTFLOAT rect[] = {{-1,-1},
                     {-1,1},
                     {1,1},
                     {1,-1}};

    float colors[] = {1,0,0,
                    0,1,0,
                    0,0,1,
                    1,1,0};

    GLuint index[] = {1,2,3, 3,0,1};

    glVertexPointer(2, GL_FLOAT, 0, &rect);
    glEnableClientState(GL_VERTEX_ARRAY);

    glColorPointer(3,GL_FLOAT, 0, &colors);
    glEnableClientState(GL_COLOR_ARRAY);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        //glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, &index);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void paintSymbolZ(){
    POINTFLOAT symbol[] = {{-1,0.99},
                        {1,0.99},
                        {1,0.99},
                        {-1,-0.99},
                        {-1,-0.99},
                        {1,-0.99}};

    float colors[] = {1,0,0,
                    0,1,0,
                    0,1,0,
                    0,0,1,
                    0,0,1,
                    1,1,0};

    void paintSymbolZpart1(){
        glVertexPointer(2, GL_FLOAT, 0, &symbol);
        glEnableClientState(GL_VERTEX_ARRAY);

        glColorPointer(3, GL_FLOAT, 0, &colors);
        glEnableClientState(GL_COLOR_ARRAY);

        glDrawArrays(GL_LINES, 0, 6);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    void paintSymbolZpart2(){
        GLuint indexZ[] = {0,1, 1,4, 4,5};

        glVertexPointer(2, GL_FLOAT, 0, &symbol);
        glEnableClientState(GL_VERTEX_ARRAY);

        glColorPointer(3, GL_FLOAT, 0, &colors);
        glEnableClientState(GL_COLOR_ARRAY);

        glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, &indexZ);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    glLineWidth(4);
    paintSymbolZpart2();
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
            glClearColor(0.0f, 0.2f, 0.4f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            pine.initPine = &initPine;
            pine.paintPineWithUseVertexArray = &paintPineWithUseVertexArray;

            pine.initPine();
            pine.paintPineWithUseVertexArray();



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
