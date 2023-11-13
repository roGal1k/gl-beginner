#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#define M_PI 3.1415926535
#define MAP_W 10
#define MAP_H 10

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

typedef struct{
    BOOL mine;
    BOOL flag;
    BOOL open;
    int cntArround;
} TCell;

TCell map[MAP_W][MAP_H];
int mines;
int closedCell;
BOOL over;

BOOL isCellInMap(int x, int y){
    return (x>=0) && (y>=0) && (x<=MAP_W) && (y<=MAP_H);
}

void convertFromScreenToOGL(HWND hwnd, int x, int y, float *ox, float *oy){
    RECT rct;
    GetClientRect(hwnd, &rct);
    *ox = x / (float) rct.right * MAP_W;
    *oy = MAP_H - y / (float) rct.bottom * MAP_H;
}

void newGame()
{
    printf("Game is start\n");
    srand(time(NULL));
    memset(map, 0, sizeof(map));

    mines = 1;
    closedCell = MAP_W * MAP_H;
    over = FALSE;
    for (int i=0; i<mines; i++)
    {
        int x = rand() % MAP_W;
        int y = rand() % MAP_H;
        if (map[x][y].mine)i--;
            else {
                    map[x][y].mine = TRUE;

                    for(int dx=-1; dx<2; dx++)
                    {
                        for(int dy=-1; dy<2; dy++)
                        {
                            if(x+dx!= MAP_W && y+dy!= MAP_H && isCellInMap(x+dx, y+dy)) map[x+dx][y+dy].cntArround +=1;
                        }
                    }
            }
    }
}

void openFields(x,y){
    if (!isCellInMap(x,y) || map[x][y].open) return;
    map[x][y].open = TRUE;
    if(map[x][y].cntArround == 0)
        for (int dx = -1; dx <2; dx++)
            for (int dy = -1; dy <2; dy++)
                if(x+dx!= MAP_W && y+dy!= MAP_H && !map[x+dx][y+dy].mine)
                    openFields(x+dx, y+dy);
    if(map[x][y].mine)
    {
        over = TRUE;
        printf("Game is lose\n");
        for (int i = 0; i < MAP_W; i++)
            for (int j = 0; j < MAP_H; j++)
                map[i][j].open = TRUE;
    }
}

void paintCount(int count){
    void line(float x1, float y1, float x2, float y2){
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    }

    glLineWidth(2);
    glColor3f(1,1,0);
    glBegin(GL_LINES);
        if (count == 0 || count == 2 || count ==3 || count ==5 || count ==6 || count ==7 || count ==8 ) line(0.2, 0.8, 0.8, 0.8);
        if (count == 0 || count == 4 || count ==5 || count ==6 || count ==8 ) line(0.2, 0.8, 0.2, 0.5);
        if (count == 0 || count == 2 || count ==6 || count ==8 ) line(0.2, 0.5, 0.2, 0.2);
        if (count == 0 || count == 1 || count ==2 || count ==3 || count ==4 || count ==7 || count ==8 ) line(0.8, 0.8, 0.8, 0.5);
        if (count == 0 || count == 1 || count ==3 || count ==4 || count ==5 || count ==6 || count ==7 || count ==8 ) line(0.8, 0.5, 0.8, 0.2);
        if (count == 2 || count == 3 || count ==4 || count ==5 || count ==6 || count ==8 ) line(0.2, 0.5, 0.8, 0.5);
        if (count == 0 || count == 2 || count ==3 || count ==5 || count ==6 || count ==8 ) line(0.2, 0.2, 0.8, 0.2);
    glEnd();
}

void paintMine()
{
    glBegin(GL_TRIANGLE_FAN);
        glColor3f( 0,0,0);
        glVertex2f(0.3, 0.3);
        glVertex2f(0.3, 0.7);
        glVertex2f(0.7, 0.7);
        glVertex2f(0.7, 0.3);
    glEnd();
}

void paintField()
{
    glBegin(GL_TRIANGLE_STRIP);
        glColor3f(0.8, 0.8, 0.8); glVertex2f(0,1);
        glColor3f(0.7, 0.7, 0.7); glVertex2f(1,1); glVertex2f(0,0);
        glColor3f(0.6, 0.6, 0.6); glVertex2f(1,0);
    glEnd();
}

void paintFieldOpen()
{
    glBegin(GL_TRIANGLE_STRIP);
        glColor3f(0.3, 0.7, 0.3); glVertex2f(0,1);
        glColor3f(0.3, 0.6, 0.3); glVertex2f(1,1); glVertex2f(0,0);
        glColor3f(0.3, 0.5, 0.3); glVertex2f(1,0);
    glEnd();
}

void paintFlag()
{
    glBegin(GL_TRIANGLES);
        glColor3f(1,0,0);
        glVertex2f(0.25, 0.75);
        glVertex2f(0.85, 0.5);
        glVertex2f(0.25, 0.25);
    glEnd();
    glLineWidth(3);
        glBegin(GL_LINES);
        glColor3f(0,0,0);
        glVertex2f(0.25, 0.75);
        glVertex2f(0.25, 0.0);
    glEnd();
}

void paintGame(){
    glLoadIdentity();
        glScalef(2.0/MAP_W, 2.0/MAP_H, 1);
        glTranslatef(-0.5*MAP_W, -0.5*MAP_H, 0);

        for(int i=0; i < MAP_W; i++)
        {
            for(int j=0; j < MAP_H; j++)
            {
                glPushMatrix();
                    glTranslatef(i, j, 0);

                    if(map[i][j].open)
                    {
                        paintFieldOpen();
                        if(map[i][j].mine) paintMine();
                        else {
                            glPushMatrix();
                                if (map[i][j].cntArround>0) paintCount(map[i][j].cntArround);
                            glPopMatrix();
                        }
                    }
                    else{
                        paintField();
                        if (map[i][j].flag) paintFlag();
                    }
                glPopMatrix();
            }
        }
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
                          1000,
                          1000,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);


    newGame();

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
            glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            if (mines == closedCell){
                newGame();
                printf("Game is win\n");
            }
            paintGame();

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

        case WM_LBUTTONDOWN:
            {
                POINTFLOAT pf;
                convertFromScreenToOGL(hwnd, LOWORD(lParam), HIWORD(lParam), &pf.x, &pf.y);
                int x = (int) pf.x;
                int y = (int) pf.y;
                if (!over) {if (isCellInMap(x,y) && !map[x][y].flag) openFields(x,y);}
                    else newGame();
            }
        break;
        case WM_RBUTTONDOWN:
            {
                POINTFLOAT pf;
                convertFromScreenToOGL(hwnd, LOWORD(lParam), HIWORD(lParam), &pf.x, &pf.y);
                int x = (int) pf.x;
                int y = (int) pf.y;
                if (isCellInMap(x,y) && !map[x][y].open) map[x][y].flag = !map[x][y].flag;
            }
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

