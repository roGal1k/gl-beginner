#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#define M_PI 3.1415926535

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

POINTFLOAT *graph = NULL;
int graphSize = 0;
float scaleY =1;
float curX;

void paintAxis(){
    glLineWidth(1);
    float d = 0.025;
    glBegin(GL_LINES);
        glColor3f(0,0,0);

        glVertex2d(-1,0); glVertex2d(1,0);
        glVertex2d(0,-1); glVertex2d(0,1);

        glVertex2d(1,0); glVertex2d(1-2*d,0+d);
        glVertex2d(1,0); glVertex2d(1-2*d,0-d);

        glVertex2d(0,1); glVertex2d(0+d,1-2*d);
        glVertex2d(0,1); glVertex2d(0-d,1-2*d);
    glEnd();
}

void graphInit(float start, float end){
    graph = realloc(graph, sizeof(*graph) * graphSize);
    float dx = (end-start)/(graphSize - 1);
    for(int i=0; i<graphSize; i++)
    {
        graph[i].x = start;
        graph[i].y = sin(start);
        start += dx;
        printf("GRAPH: %f %f\n", graph[i].x, graph[i].y);
    }
}

void paintGraph(){

    float sx = 2.0 / (graph[graphSize-1].x - graph[0].x);
    float dx = (graph[graphSize-1].x + graph[0].x) / 2;

    glPushMatrix();
    glScalef(sx, scaleY, 1);
    glTranslatef(-dx,0,0);

    glBegin(GL_LINE_STRIP);
        glColor3f(1,1,1);
        if (graphSize!=0)
            for(int i=0; i<graphSize; i++)
            {
                glVertex2d(graph[i].x, graph[i].y);
            }
    glEnd();

    glPopMatrix();
}

void addPoint( float x, float y)
{
    for(int i = 1; i< graphSize; i++)
        graph[i-1] = graph[i];
    graph[graphSize-1].x = x;
    graph[graphSize-1].y = y;
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

    graphSize = 100;
    printf("Length graph: %f\n", graph);

    curX =10;
    graphInit(-10,curX);

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

            paintAxis();

            curX +=0.1;
            addPoint(curX, sin(curX));
            paintGraph();

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

        case WM_MOUSEWHEEL:
            if ((int)wParam > 0) scaleY *= 1.5;
            else scaleY *=0.7;
            if (scaleY < 0.02) scaleY = 0.02;
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

