#pragma once

#include "entity.h"

typedef struct {
    Entity* entities;
    Entity* startEntities;
    int nextId;
} Scene;

Scene*  SceneInit();
void    SceneStart(AppContext* _app, Scene** _scene);
void    SceneUpdate(AppContext* _app, Scene** _scene);
void    SceneDraw(AppContext* _app, Scene** _scene);
Scene*  SceneFree(Scene** _scene);

Entity* Spawn(Scene** _scene);
void    Destroy(AppContext* _app, Scene** _scene, int _id);

Entity* GetEntity(Scene** _scene, int _id);
