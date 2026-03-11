#pragma once
#include <stdbool.h>
#include "types.h"

typedef enum {
    STATIC_DRAW,
    DYNAMIC_DRAW
} DrawInfo;

typedef struct {
    u32 VAO;
    u32 VBO;
    u32 EBO;
    f32* vertices;
    u32* indices;
    DrawInfo info;
} Model;

// Loads OBJ triangles with "v/vt/vn" faces into vertices:
// [pos.x, pos.y, pos.z, uv.x, uv.y] later that class will use normals
bool LoadOBJ(const char* _path, f32** _vertices, u32** _indices);

Model BuildModel(f32** _vertices, u32** _indices, DrawInfo _info);
void DrawModel(Model _model);
void FreeModel(Model _model);
