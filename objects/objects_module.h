#ifndef OBJECTS_MODULE_H_INCLUDED
#define OBJECTS_MODULE_H_INCLUDED

#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>

typedef float vec3[3];
typedef float vec2[2];

BOOL loadOBJ(
   const char *filePath,
   int out_vertices[],
   int out_uvs[],
   int out_normals[]
);

#endif // OBJECTS_MODULE_H_INCLUDED
