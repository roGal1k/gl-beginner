#include "main.h"

//!-----------------------------------------------------PRESETS
BOOL selectMode = FALSE;
TCell map[mapW][mapH];
TUV mapUV[mapW][mapH];
TCell mapNormals[mapW][mapH];
GLuint mapIndexes[mapW-1][mapH-1][6];
int mapIndexesCount = sizeof(mapIndexes)/sizeof(GLuint);

//!-----------------------------------------------------HELPERS
float mapGetHeight(float x, float y){
    if(!isCoordInMap(x,y)) return 0;

    int cX = (int)x;
    int cY = (int)y;

    float h1 = (1-(x-cX))*map[cX][cY].z + (x-cX)*map[cX+1][cY].z;
    float h2 = (1-(x-cX))*map[cX][cY+1].z + (x-cX)*map[cX+1][cY+1].z;
    float h = (1-(y-cY))*h1 + (y-cY)*h2;

    return h;
}

//!-----------------------------------------------------ANIMATION
void AnimSet(TAnim *animation, TObject *obj){
    if (animation->obj != NULL) return;
    animation->obj = obj;
    animation->cnt = 10;
    animation->dx = (camera.x - obj->x) / (float) animation->cnt;
    animation->dy = (camera.y - obj->y) / (float) animation->cnt;
    animation->dz = (camera.z - obj->scale - 0.2 - obj->z) / (float) animation->cnt;
}

void AnimMove(TAnim *animation){
    if(animation->obj != NULL){
        animation->obj->x += animation->dx;
        animation->obj->y += animation->dy;
        animation->obj->z += animation->dz;
        animation->cnt--;
        if(animation->cnt <1){

            int i;
            if(bag[selectedItem].type<0) bag[selectedItem].type = animation->obj->type;
            else {for(i=0; i<bagSize; i++)
                if(bag[i].type<0)
                {
                    bag[i].type = animation->obj->type;
                    break;
                }
            }
            if(i<bagSize){
                        animation->obj->x = rand() % mapW;
                        animation->obj->y = rand() % mapH;
            }
            animation->obj->z = mapGetHeight(animation->obj->x, animation->obj->y);
            animation->obj = NULL;
        }
    }
}

//!-----------------------------------------------------TREES
void initWoods(int woodSize){
    woodsSize = woodSize;
    woods = realloc(woods, sizeof(*woods)*woodsSize);

    for (int i=0; i < woodsSize; i++)
    {
        creatTrees(woods+i, texBirch);
    }
};

void creatTrees(TWood *obj, int type){
    obj->type = type;
    int logs = rand() % (6 + 1 - 4) + 4;
    int leafs = 5*5*2 - 2 + 3*3*2;

    obj->objCount = logs + leafs;
    obj->obj = malloc(sizeof(TObject) * obj->objCount);

    int x = rand() % mapW;
    int y = rand() % mapH;
    float z = mapGetHeight(x+0.5, y+0.5) - 0.5;

    for (int i =0; i < logs; i++)
    {
        obj->obj[i].type = 1;
        obj->obj[i].x = x;
        obj->obj[i].y = y;
        obj->obj[i].z = z+i;
    }

    int position = logs;
    for (int k =0; k< 2; k++)
    {
        for (int i= x-2; i<=x+2; i++)
        {
            for (int j= y-2; j<=y+2; j++)
            {
                if((i != x)||(j != y))
                {
                    obj->obj[position].type = 2;
                    obj->obj[position].x = i;
                    obj->obj[position].y = j;
                    obj->obj[position].z = z + logs - 2 + k;
                    position++;
                }
            }
        }
    }

    for (int k =0; k< 2; k++)
    {
        for (int i= x-1; i<=x+1; i++)
        {
            for (int j= y-1; j<=y+1; j++)
            {
                if(k!=1)
                {
                    obj->obj[position].type = 2;
                    obj->obj[position].x = i;
                    obj->obj[position].y = j;
                    obj->obj[position].z = z + logs + k;
                }
                else
                {
                    if(
                    !(i==x-1 && j==y-1) &&
                    !(i==x-1 && j==y+1) &&
                    !(i==x+1 && j==y-1) &&
                    !(i==x+1 && j==y+1))
                    {
                        obj->obj[position].type = 2;
                        obj->obj[position].x = i;
                        obj->obj[position].y = j;
                        obj->obj[position].z = z + logs + k;
                    }
                }
                position++;
            }
        }
    }

}

void paintTrees(TWood wood){

    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, cubeWood);
        glColor3f(0.7,0.7,0.7);
        glNormal3f(0,0,1);

        glBindTexture(GL_TEXTURE_2D, wood.type);
        for(int j=0; j<wood.objCount; j++)
        {
            if(wood.obj[j].type == 1) glTexCoordPointer(2, GL_FLOAT, 0, cubeUVlog);
            else glTexCoordPointer(2, GL_FLOAT, 0, cubeUVleaf);
            glPushMatrix();
                glTranslatef(wood.obj[j].x, wood.obj[j].y, wood.obj[j].z);
                glDrawElements(GL_TRIANGLES, cubeIndCount, GL_UNSIGNED_INT, cubeInd);
            glPopMatrix();
        }

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

}

//!-----------------------------------------------------HUNGER
void showHunger(int leftX, int topY, float scale){

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, hungerInd);
        glTexCoordPointer(2, GL_FLOAT, 0, hungerUV);
        glColor3f(0.7,0.7,0.7);

        for(int i = 0; i < MAXHUNGER; i++){
            glPushMatrix();
                glEnable(GL_TEXTURE_2D);
                glTranslatef(leftX + i*scale, topY, 0);
                glScalef(scale, scale, 1);
                glBindTexture(GL_TEXTURE_2D, texHunger);
                //glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glDrawElements(GL_TRIANGLE_FAN, hungerIndexesCount, GL_UNSIGNED_INT, hungerInd);
            glPopMatrix();
        }
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    /*glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, hungerInd);
        for(int i = 0; i < MAXHUNGER; i++){
            glPushMatrix();
                glTranslatef(leftX + i*scale, topY, 0);
                glScalef(scale, scale, 1);
                glColor3f(0.7,0.7,0.7);
                glDisable(GL_TEXTURE_2D);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glPopMatrix();
        }
    glDisableClientState(GL_VERTEX_ARRAY);*/

}

//!-----------------------------------------------------BAG
void showBag(int leftX, int topY, float scale){
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, bagRect);
        glTexCoordPointer(2, GL_FLOAT,0, bagRectUV);
            for(int i = 0; i < bagSize; i++){
                glPushMatrix();
                    glEnable(GL_BLEND);
                        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

                        glTranslatef(leftX + i*scale, topY, 0);
                        glScalef(scale, scale, 1);
                        glColor4ub(110,95,73,160);
                        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                    glDisable(GL_BLEND);

                    if(bag[i].type>0){
                        glPushMatrix();
                            glColor3f(1,1,1);
                            glEnable(GL_TEXTURE_2D);
                            glBindTexture(GL_TEXTURE_2D, bag[i].type);
                            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                        glPopMatrix();
                    }

                    glColor3ub(160,146,116);
                    glLineWidth(3);
                    glDisable(GL_TEXTURE_2D);
                    glDrawArrays(GL_LINE_LOOP, 0, 4);

                glPopMatrix();
            }

            glPushMatrix();
                glTranslatef(leftX-2 + selectedItem*scale, topY-1, 0);
                glScalef(scale, scale, 1);
                glColor3f(1,1,1);
                glLineWidth(4);
                glDisable(GL_TEXTURE_2D);
                glDrawArrays(GL_LINE_LOOP, 0, 4);
            glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

//!-----------------------------------------------------EFFECTSTIMER
void showEffectsTimer(int leftX, int topY, float scale){
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, bagRect);
        glTexCoordPointer(2, GL_FLOAT,0, bagRectUV);
            for(int i = 0; i < EFFECTTIMERSIZE; i++){
                glPushMatrix();
                    glEnable(GL_BLEND);
                        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

                        glTranslatef(leftX + i*scale, topY, 0);
                        glScalef(scale, scale/4, 1);

                        if (i<countEffectTime) glColor4ub(77, 140, 27, 140);
                        else glColor4ub(64, 64, 55,160);

                        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                    glDisable(GL_BLEND);

                    glColor3ub(43, 43, 39);
                    glLineWidth(3);
                    glDisable(GL_TEXTURE_2D);
                    glDrawArrays(GL_LINE_LOOP, 0, 4);

                glPopMatrix();
            }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

//!-----------------------------------------------------HEALTH
void showHealth(int leftX, int topY, float scale){
    glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, heart);
        for(int i = 0; i < MAXHEALTH; i++){
            glPushMatrix();
                glTranslatef(leftX + i*scale, topY, 0);
                glScalef(scale, scale, 1);
                if(i<health) glColor3ub(255,0,0);
                else glColor3ub(0,0,0);
                glDisable(GL_TEXTURE_2D);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
            glPopMatrix();
        }
    glDisableClientState(GL_VERTEX_ARRAY);
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

//!-----------------------------------------------------MANIPULATOR
void gameMove(){
    playerMove();
}

void playerMove(){
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

void playerTaking(HWND hwnd){
    selectMode = TRUE;
    gameShow();
    selectMode = FALSE;

    if(bag[selectedItem].type!=-1 && health != MAXHEALTH){bag[selectedItem].type = -1; health++; gameShow(); return;}

    RECT rct;
    GLubyte color[3];
    GetClientRect(hwnd, &rct);
    glReadPixels(rct.right/2.0, rct.bottom /2.0, 1,1, GL_RGB,
                 GL_UNSIGNED_BYTE, color);
    if(color[0]>0)
    {
        for(int i = 0; i < selectArrayCount; i++)
        {
            if(selectArray[i].colorIndex == color[0])
            {
                AnimSet(&animation, nature + selectArray[i].plantsArrayIndex);
            }
        }
    }
}

//!-----------------------------------------------------MAP
void initMap(){
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.5);

    health = 8;
    countHunger = 8;

    for(int i = 0; i < bagSize; i++){
        bag[i].type = -1;
    }

    loadTexture("textures/poppy.png", &texPoppy);
    loadTexture("textures/chamomile.png", &texChamomile);
    loadTexture("textures/toadstool.png", &texToadstool);
    loadTexture("textures/pole.png", &texTerain);
    loadTexture("textures/grass.png", &texGrass);
    loadTexture("textures/birch.png", &texBirch);
    loadTexture("textures/hunger_icons.png",&texHunger);


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
            map[i][j].z = (rand() %10) *0.02;

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

//initialization plants
    int grassCounterSize = 2000;
    int flowersCounterSize = 500;
    int mashroomsCounterSize = 300;
    initPlants(grassCounterSize, flowersCounterSize, mashroomsCounterSize);

//initialization woods
    int woodSize = 50;
    initWoods(woodSize);
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
void initPlants(int grassCounterSize, int flowersCounterSize, int mashroomsCounterSize){
    flowersSize = flowersCounterSize;
    grassSize = grassCounterSize;
    mashroomsSize = mashroomsCounterSize;
    nature = realloc(nature, sizeof(*nature)*(grassSize + flowersSize + mashroomsSize));

    for(int i = 0; i< grassSize + flowersSize + mashroomsSize; i++){
        if (i<grassCounterSize)
        {
            nature[i].type = texGrass;
            nature[i].scale = 1 ;
            nature[i].x = rand() % mapW;
            nature[i].y = rand() % mapH;
            nature[i].z = mapGetHeight(nature[i].x, nature[i].y);
        }
        else if (i<flowersCounterSize+grassCounterSize)
        {
            int k_iter= i-grassSize;
            if(i%2==0){
                nature[k_iter].type = texPoppy;
                nature[k_iter].scale = 0.7;
            }
            else
            {
                nature[k_iter].type = texChamomile;
                nature[k_iter].scale = 0.7;
            }
            nature[k_iter].x = rand() % mapW;
            nature[k_iter].y = rand() % mapH;
            nature[k_iter].z = mapGetHeight(nature[k_iter].x, nature[k_iter].y);
        }
        else if (i<flowersCounterSize+grassCounterSize+ mashroomsCounterSize)
        {
            int k_iter= i-grassSize-flowersSize;
            nature[k_iter].type = texToadstool;
            nature[k_iter].scale = 0.4;
            nature[k_iter].x = rand() % mapW;
            nature[k_iter].y = rand() % mapH;
            nature[k_iter].z = mapGetHeight(nature[k_iter].x, nature[k_iter].y);
        }
    }
}

void paintPlants(TObject *obj, int size){
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, object);
        glTexCoordPointer(2, GL_FLOAT, 0, objectUV);
        glNormal3f(0,0,1);

        int selectColor = 1;

        for (int i=0; i < size; i++)
        {
            static int radius =10;
            if (selectMode)
            {
                if ((obj[i].x > camera.x - radius) &&
                    (obj[i].x < camera.x + radius) &&
                    (obj[i].y > camera.y - radius) &&
                    (obj[i].y < camera.y + radius))
                {
                    glColor3ub(selectColor,0,0);
                    selectArray[selectArrayCount].colorIndex = selectColor;
                    selectArray[selectArrayCount].plantsArrayIndex = i;
                    selectArrayCount++;
                    selectColor++;
                    if (selectColor >=255) break;
                }
                else continue;
            }
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

//!-----------------------------------------------------GAMEMENU
void menuShow(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,scrSize.x, scrSize.y, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    float itemSize = (scrSize.x/2)/bagSize;
    float heartSize = itemSize/3;
    float timercountSize = (itemSize*bagSize)/EFFECTTIMERSIZE;

    showBag(scrSize.x/4, scrSize.y -itemSize -10, itemSize);
    showEffectsTimer(scrSize.x/4, scrSize.y -itemSize -10 -timercountSize/4 -5, timercountSize);
    showHealth(scrSize.x/4, scrSize.y -itemSize -10 -timercountSize/4 -5 -heartSize -5, heartSize);
    showHunger(scrSize.x/4 + itemSize*bagSize - heartSize*MAXHUNGER, scrSize.y -itemSize -10 -timercountSize/4 -5 -heartSize -5, heartSize);
}

//!-----------------------------------------------------GAME
void initGame(){
    glEnable(GL_DEPTH_TEST);
    initMap();
    RECT rct;
    GetClientRect(hwnd, &rct);
    windResize(rct.right, rct.bottom);
}

void gameShow(){
    float sz = 0.1;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-scrKoef*sz, scrKoef*sz, -sz, sz, sz*2, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);

    static float alfa = 0;
    alfa += 0.125;
    if(alfa > 180) alfa -= 360;

    gameMove();

    #define abs(a) ((a) > 0 ? (a) : -(a))
    float kcc = 1 - (abs(alfa)/180);

    #define zakat 60
    float nigthmareKoeef = 90 - abs(alfa);
    nigthmareKoeef = (zakat - abs(nigthmareKoeef));
    nigthmareKoeef = nigthmareKoeef < 0 ? 0 : nigthmareKoeef/zakat;

    if(selectMode) glClearColor(0,0,0,0);
    else glClearColor(0.6 * kcc, 0.8 * kcc, 1 * kcc, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(selectMode){
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
    }
    else{
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
    }

    AnimMove(&animation);

    glPushMatrix();
        if(!selectMode) // paint sun
        {
            glPushMatrix();
                glRotatef(-camera.xRot, 1, 0, 0);
                glRotatef(-camera.zRot, 0, 0, 1);
                glRotatef(alfa, 0, 1,0);
                glTranslatef(0,0,20);

                glDisable(GL_DEPTH_TEST);
                glDisable(GL_TEXTURE_2D);
                    glColor3f(1, 1 - nigthmareKoeef*0.4, 1-0.6*nigthmareKoeef);
                    glEnableClientState(GL_VERTEX_ARRAY);
                        glVertexPointer(3, GL_FLOAT, 0, sun);
                        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                    glDisableClientState(GL_VERTEX_ARRAY);
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();
        }

        applyCamera();

            //world lightning
        glPushMatrix();
            glRotatef(alfa, 0, 1,0);
            GLfloat position[] = {0,0,1,0};
            glLightfv(GL_LIGHT0, GL_POSITION, position);
            float lightIntensive[] = {1+nigthmareKoeef*1.5,1,1,0};
            glLightfv(GL_LIGHT0,GL_DIFFUSE, lightIntensive);

            float clr = kcc*0.15 +0.15;
            float intensiveBackgroundLightning[] = {clr, clr, clr, 0};
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, intensiveBackgroundLightning);
        glPopMatrix();

        if(!selectMode){ // pain terrain
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
                glVertexPointer(3, GL_FLOAT, 0, map);
                glTexCoordPointer(2, GL_FLOAT, 0, mapUV);
                glNormalPointer(GL_FLOAT, 0, mapNormals);

                glPushMatrix();
                    glColor3f(0.7,0.7,0.7);
                    glBindTexture(GL_TEXTURE_2D, texTerain);
                    glDrawElements(GL_TRIANGLES, mapIndexesCount, GL_UNSIGNED_INT, mapIndexes);

                glPopMatrix();
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
        }

        if(!selectMode) // paint trees
        {
            for(int i=0; i<woodsSize; i++)
            {
                paintTrees(woods[i]);
            }
        }

        selectArrayCount = 0;
        paintPlants(nature, grassSize + flowersSize + mashroomsSize);
    glPopMatrix();
}

//!-----------------------------------------------------WNDSETTINGS
void windResize(int x, int y){
    glViewport(0, 0, x, y);
    scrSize.x = x;
    scrSize.y = y;
    scrKoef  = x / (float) y;
};

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
            menuShow();
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
            playerTaking(hwnd);
        break;

        case WM_MOUSEWHEEL:
            if (GET_WHEEL_DELTA_WPARAM(wParam)>0){
                if (selectedItem<bagSize-1)selectedItem++;
                else selectedItem=0;
            }
            else{
                if (selectedItem>0)selectedItem--;
                else selectedItem=bagSize-1;
            }
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
