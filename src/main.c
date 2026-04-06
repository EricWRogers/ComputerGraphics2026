#ifdef _WIN32
#include <windows.h>
#endif

#include <math.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "cpup/io.h"
#include "cpup/vec.h"
#include "cpup/math.h"
#include "cpup/types.h"
#include "cpup/model.h"
#include "cpup/shader.h"
#include "cpup/window.h"
#include "cpup/inputmanager.h"

#include "cpup/scene.h"

#include "cube.h"
#include "light.h"

AppContext app;

static Entity* SpawnPointLight(
    Scene** _scene,
    Model* _model,
    u32 _shaderId,
    Image* _image,
    const char* _name,
    Vector3 _position,
    Vector3 _lightColor,
    f32 _intensity)
{
    Entity* light = Spawn(_scene);
    Light* lightData = calloc(1, sizeof(Light));

    light->name = (char*)_name;
    light->transform.position = _position;
    light->transform.scale = InitVector3(5.0f, 5.0f, 5.0f);
    light->color = InitVector4(_lightColor.x, _lightColor.y, _lightColor.z, 1.0f);
    lightData->enabled = true;
    lightData->color = _lightColor;
    lightData->intensity = _intensity;
    lightData->ambientStrength = 0.04f;
    lightData->specularStrength = 0.3f;
    lightData->rotationSpeed = 0.0f;
    light->data = lightData;
    light->image = _image;
    light->model = _model;
    light->shaderId = _shaderId;
    light->Start = LightStart;
    light->Update = LightUpdate;
    light->Draw = LightDraw;
    light->OnDestroy = LightOnDestroy;
    return light;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (InitCanis() > 0)
        return 1;

    app.windowWidth = 600;
    app.windowHeight = 600;

    if (InitWindow(&app) > 0)
        return 1;

    app.fogColor = InitVector3(0.2f, 0.3f, 0.3f);
    app.fogNear = 100.0f;
    app.fogFar = 250.0f;

    Scene *scene = SceneInit();
    app.scene = scene;

    Image iconImage = IOLoadImage("assets/textures/canis_engine_icon.tga");
    Image containerImage = IOLoadImage("assets/textures/container.tga");
    Image circleImage = IOLoadImage("assets/textures/circle.tga");
    Image squareImage = IOLoadImage("assets/textures/square.tga");
    Image cubeImage = IOLoadImage("assets/textures/cube_base_color.tga");
    Image gridImage = IOLoadImage("assets/textures/grid.tga");
    Image noiseImage = IOLoadImage("assets/textures/noise.tga");
    Image awesomeImage = IOLoadImage("assets/textures/awesome_face.tga");

    // build and compile our shader program
    u32 shaderProgram = GenerateShaderFromFiles("assets/shaders/logo.vs", "assets/shaders/logo.fs");
    u32 ghostShader = GenerateShaderFromFiles("assets/shaders/ghost.vs", "assets/shaders/ghost.fs");

    float ve[] = {
        // positions            // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top left
    };
    unsigned int in[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    f32 *vertices = vec_init(32, sizeof(f32));
    vec_append(&vertices, ve, 32);

    u32 *indices = vec_init(6, sizeof(u32));
    vec_append(&indices, in, 6);

    Model model = BuildModel(&vertices, &indices, STATIC_DRAW);

    f32 *cubeVertices = NULL;
    u32 *cubeIndices = NULL;
    Model cubeModel = {0};

    if (LoadOBJ("assets/models/cube_overlaping_uv.obj", &cubeVertices, &cubeIndices))
    {
        cubeModel = BuildModel(&cubeVertices, &cubeIndices, STATIC_DRAW);
    }
    else
    {
        printf("Warning: Failed to load cube model, using quad mesh for cube entity\n");
    }

    Entity *cube = Spawn(&scene);
    cube->name = "cube";
    cube->transform.position = InitVector3(app.windowWidth * 0.5f, app.windowHeight * 0.5f, -250.0f);
    cube->transform.scale = InitVector3(180.0f, 180.0f, 180.0f);
    cube->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
    cube->data = calloc(1, sizeof(Cube));
    ((Cube *)cube->data)->rotationSpeed = 35.0f;
    ((Cube *)cube->data)->noiseImage = &noiseImage;
    cube->image = &containerImage;
    cube->model = &cubeModel;
    cube->shaderId = shaderProgram;
    cube->Start = CubeStart;
    cube->Update = CubeUpdate;
    cube->Draw = CubeDraw;
    cube->OnDestroy = CubeOnDestroy;

    SpawnPointLight(
        &scene,
        &cubeModel,
        ghostShader,
        &circleImage,
        "warmLight",
        InitVector3(420.0f, 300.0f, -140.0f),
        InitVector3(1.0f, 0.82f, 0.52f),
        3.0f);

    SpawnPointLight(
        &scene,
        &cubeModel,
        ghostShader,
        &circleImage,
        "cyanLight",
        InitVector3(300.0f, 300.0f, -120.0f),
        InitVector3(0.0f, 0.0f, 1.0f),
        3.0f);

    SpawnPointLight(
        &scene,
        &cubeModel,
        ghostShader,
        &circleImage,
        "magentaLight",
        InitVector3(180.0f, 300.0f, -140.0f),
        InitVector3(1.0f, 0.35f, 0.80f),
        3.0f);

    SpawnPointLight(
        &scene,
        &cubeModel,
        ghostShader,
        &circleImage,
        "greenLight",
        InitVector3(300.0f, 420.0f, -120.0f),
        InitVector3(0.45f, 1.0f, 0.55f),
        3.0f);

    bool running = true;
    f32 time = 0.0f;
    while (running)
    {
        // imput
        InputManagerNewFrame(&app);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // render
        ClearWindow(&app);

        if (app.time != 0.0f)
            app.deltaTime = (SDL_GetTicksNS() * 1e-9) - app.time;

        app.time = SDL_GetTicksNS() * 1e-9;

        SceneStart(&app, &scene);

        // app.projection = Mat4Orthographic(0.0f, (float)app.windowWidth, 0.0f, (float)app.windowHeight, 0.001f, 1000.0f);

        f32 fov = 60.0f * DEG2RAD;
        f32 aspectR = (f32)app.windowWidth / (f32)app.windowHeight;

        app.projection = Mat4Perspective(fov, aspectR, 0.1f, 1000.0f);
        app.cameraPosition = InitVector3((float)app.windowWidth * 0.5f, (float)app.windowHeight * 0.5f, 0.5f);
        app.view = IdentityMatrix4();
        Mat4Translate(&app.view, Vec3Mul(app.cameraPosition, -1.0f));

        SceneUpdate(&app, &scene);
        SceneSyncLights(&app, &scene);

        SceneDraw(&app, &scene);

        SwapWindow(&app);
    }

    FreeModel(model);
    if (cubeVertices)
        FreeModel(cubeModel);

    free(iconImage.data);
    free(containerImage.data);
    free(circleImage.data);
    free(squareImage.data);
    free(cubeImage.data);
    free(gridImage.data);

    SceneFree(&scene);

    FreeWindow(&app);

    DeleteShader(shaderProgram);
    return 0;
}
