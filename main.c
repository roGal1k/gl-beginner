#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

#include "camera/camera_module.h"
#include "main.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image/stb_image.h"

// map size
#define mapW 100
#define mapH 100

//count enemies
#define enemyCnt 40

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

//Checking whether the coordinates belong to the map
BOOL isCoordInMap(float x, float y)
{
    return (x>=0 && x<mapW && y>=0 && y<mapH);
}

//!-----------------------------------------------------TEXTURES

void loadTexture(char *filename, unsigned int *target){
    int width, height, count;
    unsigned char *data = stbi_load(filename, &width, &height, &count, 0);
    glGenTextures(1, target);
    glBindTexture(GL_TEXTURE_2D, *target);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                                    0, count == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

}

//!-----------------------------------------------------NORMALIZE VECTOR
#define sqr(a) (a)*(a)
void CalcNormals(TCell a, TCell b, TCell c, TCell *n){
    float wrki;
    TCell v1,v2;

    v1.x = a.x - b.x;
    v1.y = a.y - b.y;
    v1.z = a.z - b.z;
    v2.x = b.x - c.x;
    v2.y = b.y - c.y;
    v2.z = b.z - c.z;

    n->x = (v1.y * v2.z - v1.z * v2.y);
    n->y = (v1.z * v2.x - v1.x * v2.z);
    n->z = (v1.x * v2.y - v1.y * v2.x);
    wrki = sqrt(sqr(n->x) + sqr(n->y) + sqr(n->z));
    n->x /= wrki;
    n->y /= wrki;
    n->z /= wrki;
}

//!-----------------------------------------------------PRESET
BOOL ShowMask = FALSE;
TCell map[mapW][mapH];
TUV mapUV[mapW][mapH];
TCell mapNormals[mapW][mapH];
GLuint mapIndexes[mapW-1][mapH-1][6];
int mapIndexesCount = sizeof(mapIndexes)/sizeof(GLuint);

//!-----------------------------------------------------MANIPULATOR
void playerMove();
void gameMove()
{
    playerMove();
}

float mapGetHeight(float x, float y);
void playerMove()
{
    if (GetForegroundWindow() != hwnd) return;

    float angle = -camera.zRot /180 * M_PI;
    float speed = 0;

    if(GetKeyState('W') <0 && GetKeyState(VK_SHIFT) <0) speed = 0.175;
        else if(GetKeyState('W') <0) speed = 0.1;
    if(GetKeyState('S') <0) speed = -0.1;
    if(GetKeyState('D') <0) {speed = -0.1; angle -= M_PI*0.5;};
    if(GetKeyState('A') <0) {speed = -0.1; angle += M_PI*0.5;};

    if(speed!=0)
    {
        if (!isCoordInMap(camera.x + sin(angle) * speed, camera.y + cos(angle) * speed)) return;
        camera.x += sin(angle) * speed;
        camera.y += cos(angle) * speed;
        camera.z = 1.7 + mapGetHeight(camera.x, camera.y);
    }

    float speedCam = 0.2;
    cameraAutoMoveByMouse(400, 300, speedCam);
}

float mapGetHeight(float x, float y)
{
    if(!isCoordInMap(x,y)) return 0;

    int cX = (int)x;
    int cY = (int)y;
        printf("x,y: %f%,%f, cx,cy:%f,%f\n", x, y, cX, cY);

    float h1 = (1-(x-cX))*map[cX][cY].z + (x-cX)*map[cX+1][cY].z;
    float h2 = (1-(x-cX))*map[cX][cY+1].z + (x-cX)*map[cX+1][cY+1].z;
    float h = (1-(y-cY))*h1 + (y-cY)*h2;
        printf("camera[h1,h2,h]: %f%,%f,%f\n", h1, h2, h);
    return h;
}

//!-----------------------------------------------------MAP
void mapCreateHill(int posX, int posY, int radius, int heightExtremum);

void initMap()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.99);

    loadTexture("textures/flower.png", &texFlower);
    loadTexture("textures/flower2.png", &texFlower2);
    loadTexture("textures/grib.png", &texMashroom);
    loadTexture("textures/pole.png", &texTerain);
    loadTexture("textures/trava.png", &texGrass);
    loadTexture("textures/tree.png", &texTree);
    loadTexture("textures/tree2.png", &texTree2);

    for (int i=0; i<mapW; i++){
        int pos = i*mapH;
        for (int j=0; j<mapH; j++)
        {
            if (i < mapW-1 && j < mapH-1)
            {
                mapIndexes[i][j][0] = pos;
                mapIndexes[i][j][1] = pos + 1;
                mapIndexes[i][j][2] = pos + 1 + mapH;

                mapIndexes[i][j][3] = pos + 1 + mapH;
                mapIndexes[i][j][4] = pos + mapH;
                mapIndexes[i][j][5] = pos;
                pos++;
            };
            map[i][j].x = i;
            map[i][j].y = j;
            map[i][j].z = (rand() %10) *0.05;

            mapUV[i][j].u = i;
            mapUV[i][j].v = j;
        }
    }

    for (int i=0; i<10; i++){
        int heightHill = rand() % mapW/12;
        mapCreateHill(rand()%mapW, rand()%mapH, (int)((rand() % mapW/20 )* pow(heightHill,2)), heightHill);
    }


    for (int i=0; i<mapW-1; i++){
        for (int j=0; j<mapH-1; j++){
            CalcNormals(map[i][j], map[i+1][j], map[i][j+1], &mapNormals[i][j]);
        }
    }

//init plants
    int grassCounterSize = 2000;
    int flowersCounterSize = 500;
    int treesCounterSize = 250;
    initPlants(grassCounterSize, flowersCounterSize, treesCounterSize);
}

//buff the map with hill
void mapCreateHill(int posX, int posY, int radius, int heightExtremum)
{
    if(radius==0 || (posX-radius)<0 || (posY-radius)<0) return;
    for(int i= posX - radius; i <= posX + radius; i++){
        for(int j = posY -radius; j <= posY + radius; j++){
            if(isCoordInMap(i,j)){
                float length = sqrt(pow(posX-i,2) + pow(posY-j,2));
                if(length < radius){
                    length = length / radius * M_PI_2;
                    map[i][j].z =cos(length) * heightExtremum;
                }
            }
        }
    }
}

//!-----------------------------------------------------OBJECTS
void initPlants(int grassCounterSize, int flowersCounterSize, int treesCounterSize){
    grassSize = grassCounterSize;
    grass = realloc(grass, sizeof(*grass)*grassSize);

    flowersSize = flowersCounterSize;
    flowers = realloc(flowers, sizeof(*flowers)*flowersSize);

    treesSize = treesCounterSize;
    trees = realloc(trees, sizeof(*trees)*treesSize);

    for(int i = 0; i< grassSize + flowersSize + treesSize; i++){
        if (i<grassCounterSize)
        {
            grass[i].type = texGrass;
            grass[i].scale = 1 ;
            grass[i].x = rand() % mapW;
            grass[i].y = rand() % mapH;
            grass[i].z = mapGetHeight(grass[i].x, grass[i].y);
        }
        else if (i<flowersCounterSize+grassSize)
        {
            int k_iter= i-grassSize;
            if(i%2==0){
                flowers[k_iter].type = texFlower;
                flowers[k_iter].scale = 0.8;
            }
            else
            {
                flowers[k_iter].type = texFlower2;
                flowers[k_iter].scale = 0.8;
            }
            flowers[k_iter].x = rand() % mapW;
            flowers[k_iter].y = rand() % mapH;
            flowers[k_iter].z = mapGetHeight(flowers[k_iter].x, flowers[k_iter].y);
        }
        else
        {
            int k_iter= i-grassSize-flowersCounterSize;
            if (i%2==0)
            {
                trees[k_iter].type = texTree;
                trees[k_iter].scale = 4 + (rand() %14);
            }else if (treesSize/2<i<treesSize){
                trees[k_iter].type = texTree2;
                trees[k_iter].scale = 4 + (rand() %14);
            }

            trees[k_iter].x = rand() % mapW;
            trees[k_iter].y = rand() % mapH;
            trees[k_iter].z = mapGetHeight(trees[k_iter].x, trees[k_iter].y);
        }
    }
}

void paintPlants(TObject *obj, int size){
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, object);
        glTexCoordPointer(2, GL_FLOAT, 0, objectUV);
        glNormal3f(0,0,1);
        if (ShowMask)
            glColor3f(0,0,0);
        else
            glColor3f(0.7,0.7,0.7);
        for (int i=0; i < size; i++)
        {
            glBindTexture(GL_TEXTURE_2D, obj[i].type);
            glPushMatrix();
                glTranslatef(obj[i].x, obj[i].y,  obj[i].z);
                glScalef(obj[i].scale, obj[i].scale, obj[i].scale);
                glDrawElements(GL_TRIANGLES, objectIndexesCount, GL_UNSIGNED_INT, objectIndexes);
            glPopMatrix();
        }
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

//!-----------------------------------------------------GAME
void initGame()
{
    glEnable(GL_DEPTH_TEST);
    initMap();
    RECT rct;
    GetClientRect(hwnd, &rct);
    windResize(rct.right, rct.bottom);
}

void gameShow()
{
    gameMove();

    if(ShowMask) glClearColor(0,0,0,0);
    else glClearColor(0.6, 0.8, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
        applyCamera();

        GLfloat position[] = {1,0,2,0};
        glLightfv(GL_LIGHT0, GL_POSITION, position);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, map);
        glTexCoordPointer(2, GL_FLOAT, 0, mapUV);
        glNormalPointer(GL_FLOAT, 0, mapNormals);
        glPushMatrix();
            if (ShowMask)
                glColor3f(0,0,0);
            else
                glColor3f(0.7,0.7,0.7);
            glBindTexture(GL_TEXTURE_2D, texTerain);
            glDrawElements(GL_TRIANGLES, mapIndexesCount, GL_UNSIGNED_INT, mapIndexes);
        glPopMatrix();
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        paintPlants(grass, grassSize);
        paintPlants(trees, treesSize);
        paintPlants(flowers, flowersSize);
    glPopMatrix();
}

//!-----------------------------------------------------MAIN
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
            windResize(width, height);
        break;

        case WM_LBUTTONDOWN:
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
