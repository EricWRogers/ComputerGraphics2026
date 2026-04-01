#include "light.h"

void LightStart(AppContext* _app, Entity* _entity) {}

void LightUpdate(AppContext* _app, Entity* _entity)
{
    ENTITY_DATA(Light, lightData, _entity);

    _entity->transform.rotation += lightData->rotationSpeed * _app->deltaTime;
}

void LightDraw(AppContext* _app, Entity* _entity)
{
    ENTITY_DATA(Light, lightData, _entity);

    Matrix4 transform = IdentityMatrix4();
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(1.0f, 1.0f, 0.5f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);
    ShaderSetVector3(_entity->shaderId, "FOG_COLOR", _app->fogColor);
    ShaderSetFloat(_entity->shaderId, "FOG_NEAR", _app->fogNear);
    ShaderSetFloat(_entity->shaderId, "FOG_FAR", _app->fogFar);

    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void LightOnDestroy(AppContext* _app, Entity* _entity) {}
