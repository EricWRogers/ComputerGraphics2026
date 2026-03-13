#pragma once

#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"

typedef struct {
    f32 rotationSpeed;
    Image* noiseImage;
} Cube;

void CubeStart(AppContext* _app, Entity* _entity) {}

void CubeUpdate(AppContext* _app, Entity* _entity)
{
    ENTITY_DATA(Cube, cubeData, _entity);
}

void CubeDraw(AppContext* _app, Entity* _entity)
{
    Matrix4 transform = IdentityMatrix4();
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(1.0f, 1.0f, 0.5f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void CubeOnDestroy(AppContext* _app, Entity* _entity) {}
