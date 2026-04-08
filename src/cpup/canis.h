#pragma once

#include "math.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_scancode.h>

typedef struct Scene Scene;

enum { MAX_LIGHTS = 8 };

typedef struct {
    Vector3 position;
    Vector3 color;
    f32 intensity;
} SceneLight;

typedef struct AppContext {
    void* window;
    void* glContext;
    Scene* scene;
    i32 windowWidth;
    i32 windowHeight;
    u32 postProcessShaderId;
    u32 postProcessFramebufferId;
    u32 postProcessColorTextureId;
    u32 postProcessDepthStencilRenderbufferId;
    Matrix4 projection;
    Matrix4 view;
    Vector3 cameraPosition;
    SceneLight lights[MAX_LIGHTS];
    i32 lightCount;
    Vector3 fogColor;
    f32 fogNear;
    f32 fogFar;
    f32 time;
    f32 deltaTime;
    bool previousKeys[SDL_SCANCODE_COUNT];
    bool currentKeys[SDL_SCANCODE_COUNT];
} AppContext;

extern i32 InitCanis();
