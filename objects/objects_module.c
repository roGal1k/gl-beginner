#include "objects_module.h"

GLuint vertexIndices[],
     uvIndices[],
     normalIndices[];
int vertexIndicesCount = sizeof(vertexIndices)/sizeof(GLuint);
int uvIndicesCount = sizeof(uvIndices)/sizeof(GLuint);
int normalIndicesCount = sizeof(normalIndices)/sizeof(GLuint);

vec3 temp_vertices[];
vec3 temp_uvs[];
vec3 temp_normals[];

void insertInArray(){
}

BOOL loadOBJ( const char *filePath, int out_vertices[], int out_uvs[], int out_normals[]){

    FILE *file = fopen(filePath, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return FALSE;
    }

    while( 1 ){
       char lineHeader[128];
       int res = fscanf(file, "%s", lineHeader);
       if(res == EOF)
           break;

       if(strcmp( lineHeader, "v" ) == 0 ){
           vec3 vertex;
           fscanf(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
           temp_vertices[] vertex;

        Если первое слово равно «v», значит дальше должно быть 3 float числа. Поэтому мы создаем glm::vec3 и читаем эти числа в него.

        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
           glm::vec2 uv;
           fscanf(file, "%f %f\n", &uv.x, &uv.y );
           temp_uvs.push_back(uv);

        Если не «v», а «vt», то после них должно быть 2 float числа, поэтому мы создаем glm::vec2 и читаем числа в него.

        То же самое с нормалями:
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
           glm::vec3 normal;
           fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
           temp_normals.push_back(normal);

        А вот с «f» все немного посложнее:
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
           std::string vertex1, vertex2, vertex3;
           unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
           int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
        if (matches != 9){
               printf("File can't be read by our simple parser : ( Try exporting with other options\n");
               return false;
        }
        vertexIndices.push_back(vertexIndex[0]);
        vertexIndices.push_back(vertexIndex[1]);
        vertexIndices.push_back(vertexIndex[2]);
        uvIndices    .push_back(uvIndex[0]);
        uvIndices    .push_back(uvIndex[1]);
        uvIndices    .push_back(uvIndex[2]);
        normalIndices.push_back(normalIndex[0]);
        normalIndices.push_back(normalIndex[1]);
        normalIndices.push_back(normalIndex[2]);

    return TRUE;
}
