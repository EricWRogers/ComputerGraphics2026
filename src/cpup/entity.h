#pragma once
#include "math.h"
#include "model.h"
#include "shader.h"

typedef struct {
    Vector3 position;
    float   rotation;
    Vector3 scale;
} Transform;

typedef struct {
   Vector3 ambient;
   Vector3 diffuse;
   Image* specular;
   float shininess;
} Material;

struct Image;
struct Shader;
typedef struct AppContext AppContext;

typedef struct Entity Entity; 

struct Entity {
    int         id;
    char*       name;
    Transform   transform;
    Material    material;
    u32         shaderId;
    Image*      image;
    Model*      model;
    Vector2     velocity;
    Vector4     color;
    
    void*       data;

    void (*Start)(AppContext*, Entity*);
    void (*Update)(AppContext*, Entity*);
    void (*Draw)(AppContext*, Entity*);
    void (*OnDestroy)(AppContext*, Entity*);
};

#define ENTITY_DATA(Type, varName, entity)  \
    Type* varName = (Type*)entity->data;    \
    if (varName == NULL)                    \
        return;
