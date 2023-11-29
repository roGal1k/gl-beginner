#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#define M_PI 3.1415926535

int width, height;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

typedef struct {
    char name[20];
    float vert[8];
    BOOL hover;
}TButton;

TButton btns[] = {{"start", {10,10,110,10,110,40,10,40}, FALSE},
                  {"settings", {10,50,110,50,110,80,10,80}}, FALSE};

int buttonCount = sizeof(btns)/sizeof(btns[0]);

void TButtonShow(TButton btn){
    glEnableClientState(GL_VERTEX_ARRAY);
        if (btn.hover) glColor3f(0,1,1);
        else glColor3f(1,1,0);
        glVertexPointer(2, GL_FLOAT, 0, btn.vert);
        glDrawArrays(GL_TRIANGLE_FAN, 0,4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void showMenu(){
    glPushMatrix();
        glLoadIdentity();
        glOrtho(0, width, height, 0, -1, 1);
        int i=0;
        while(i<buttonCount){TButtonShow(btns[i]);i++;};
    glPopMatrix();
};

BOOL PointInButton(int xMouse, int yMouse, TButton btn){
    return(((xMouse>btn.vert[0]) && (xMouse<btn.vert[2])) && ((yMouse>btn.vert[1]) && (yMouse<btn.vert[7])));
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
                          800,
                          600,
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

            showMenu();

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

        case WM_LBUTTONDOWN:{
            int i=0; while(i<buttonCount){
            if (PointInButton(LOWORD(lParam), HIWORD(lParam), btns[i])){printf("Click on button: %s\n", btns[i].name);}; i++;};
        }
        break;

        case WM_LBUTTONUP:
            showMenu();
        break;

        case WM_MOUSEMOVE:
        {
            for (int i=0; i<buttonCount; i++){
                btns[i].hover= PointInButton(LOWORD(lParam), HIWORD(lParam), btns[i]);
            }
        }
        break;

        case WM_SIZE:
            width = LOWORD(lParam);
            height = HIWORD(lParam);
            glViewport(0, 0, width, height);
            glLoadIdentity();
            float screenRatio = width/ (float)height;
            glOrtho(-screenRatio, screenRatio, -1,1, -1,1);
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
