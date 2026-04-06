#pragma once

#include <stdbool.h>

#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"

typedef struct {
    bool enabled;
    Vector3 color;
    f32 intensity;
    f32 rotationSpeed;
} Light;

void LightStart(AppContext* _app, Entity* _entity);
void LightUpdate(AppContext* _app, Entity* _entity);
void LightDraw(AppContext* _app, Entity* _entity);
void LightOnDestroy(AppContext* _app, Entity* _entity);
