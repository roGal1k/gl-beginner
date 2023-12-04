#include "camera_module.h"

struct MCamera camera = {0, 0, 1.7, 65, 125};

void applyCamera(){
    glRotatef(-camera.xRot, 1, 0, 0);
    glRotatef(-camera.zRot, 0, 0, 1);
    glTranslatef(-camera.x, -camera.y, -camera.z);
}

void cameraRotation(float xAngle, float zAngle){
    camera.zRot += zAngle;
    if (camera.zRot < 0) camera.zRot += 360;
    if (camera.zRot >360) camera.zRot -= 360;
    camera.xRot += xAngle;
    if (camera.xRot < 0) camera.zRot = 0;
    if (camera.xRot > 180) camera.zRot = 180;
}

void cameraAutoMoveByMouse(int midleX, int midleY, float speed){
    POINT mouseCur;
    POINT base = {midleX, midleY};
    GetCursorPos(&mouseCur);
    cameraRotation((base.y - mouseCur.y) * speed, (base.x - mouseCur.x) * speed);
    SetCursorPos(base.x, base.y);
}

void windResize(int x, int y){
    glViewport(0, 0, x, y);
    float k = x / (float)y;
    float sz = 0.1;
    glLoadIdentity();
    glFrustum(-k*sz, k*sz, -sz, sz, sz*2, 100);
};
