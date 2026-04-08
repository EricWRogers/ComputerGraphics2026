#include "postprocess.h"

#include "math.h"
#include "model.h"
#include "opengl.h"
#include "shader.h"
#include "vec.h"

#include <SDL3/SDL_log.h>

static Model gPostProcessQuad = {0};
static bool gPostProcessQuadReady = false;

static bool EnsurePostProcessQuad()
{
    if (gPostProcessQuadReady)
        return true;

    f32 quadVerticesData[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };
    u32 quadIndicesData[] = {
        0, 1, 2,
        0, 2, 3
    };

    f32* quadVertices = vec_init(20, sizeof(f32));
    u32* quadIndices = vec_init(6, sizeof(u32));

    if (quadVertices == NULL || quadIndices == NULL)
    {
        if (quadVertices != NULL)
            vec_free(&quadVertices);
        if (quadIndices != NULL)
            vec_free(&quadIndices);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to allocate the post-process quad");
        return false;
    }

    vec_append(&quadVertices, quadVerticesData, 20);
    vec_append(&quadIndices, quadIndicesData, 6);

    gPostProcessQuad = BuildModel(&quadVertices, &quadIndices, STATIC_DRAW);
    gPostProcessQuadReady = true;
    return true;
}

i32 ResizePostProcess(AppContext* _appContext, i32 _width, i32 _height)
{
    if (_appContext == NULL)
        return 1;

    if (_width <= 0 || _height <= 0)
        return 0;

    glBindFramebuffer(GL_FRAMEBUFFER, _appContext->postProcessFramebufferId);

    glBindTexture(GL_TEXTURE_2D, _appContext->postProcessColorTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _appContext->postProcessColorTextureId, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, _appContext->postProcessDepthStencilRenderbufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _appContext->postProcessDepthStencilRenderbufferId);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Post-process framebuffer is incomplete");
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return 1;
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return 0;
}

i32 InitPostProcess(AppContext* _appContext)
{
    if (_appContext == NULL)
        return 1;

    if (!EnsurePostProcessQuad())
        return 1;

    glGenFramebuffers(1, &_appContext->postProcessFramebufferId);
    glGenTextures(1, &_appContext->postProcessColorTextureId);
    glGenRenderbuffers(1, &_appContext->postProcessDepthStencilRenderbufferId);

    return ResizePostProcess(_appContext, _appContext->windowWidth, _appContext->windowHeight);
}

void BeginPostProcess(AppContext* _appContext)
{
    if (_appContext == NULL)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, _appContext->postProcessFramebufferId);
    glViewport(0, 0, _appContext->windowWidth, _appContext->windowHeight);
}

void EndPostProcess(AppContext* _appContext)
{
    if (_appContext == NULL)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _appContext->windowWidth, _appContext->windowHeight);

    glClearColor(_appContext->fogColor.x, _appContext->fogColor.y, _appContext->fogColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    BindShader(_appContext->postProcessShaderId);
    ShaderBindTexture(_appContext->postProcessShaderId, _appContext->postProcessColorTextureId, "SCREEN_TEXTURE", 0);
    ShaderSetFloat(_appContext->postProcessShaderId, "TIME", _appContext->time);
    ShaderSetVector2(_appContext->postProcessShaderId, "SCREEN_SIZE", InitVector2((f32)_appContext->windowWidth, (f32)_appContext->windowHeight));
    DrawModel(gPostProcessQuad);
    UnBindShader();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

void FreePostProcess(AppContext* _appContext)
{
    if (_appContext == NULL)
        return;

    if (_appContext->postProcessDepthStencilRenderbufferId != 0)
        glDeleteRenderbuffers(1, &_appContext->postProcessDepthStencilRenderbufferId);
    if (_appContext->postProcessColorTextureId != 0)
        glDeleteTextures(1, &_appContext->postProcessColorTextureId);
    if (_appContext->postProcessFramebufferId != 0)
        glDeleteFramebuffers(1, &_appContext->postProcessFramebufferId);

    _appContext->postProcessDepthStencilRenderbufferId = 0;
    _appContext->postProcessColorTextureId = 0;
    _appContext->postProcessFramebufferId = 0;

    if (gPostProcessQuadReady)
    {
        FreeModel(gPostProcessQuad);
        gPostProcessQuad = (Model){0};
        gPostProcessQuadReady = false;
    }
}
