#ifndef CAMERA_MODULE_H_INCLUDED
#define CAMERA_MODULE_H_INCLUDED

#include <windows.h>
#include <GL/gl.h>

struct MCamera{
    float x,y,z;
    float xRot,zRot;
}camera;

void applyCamera();
void cameraRotation(float xAngle, float zAngle);
void cameraAutoMoveByMouse(int midleX, int midleY, float speed);
void windResize(int x, int y);

#endif // CAMERA_MODULE_H_INCLUDED
