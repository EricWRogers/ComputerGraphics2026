#include "model.h"

#include <stdio.h>
#include <string.h>

#include "math.h"
#include "vec.h"
#include "opengl.h"

typedef struct {
    u32 vertex;
    u32 uv;
    u32 normal;
} ObjIndex;

static bool ObjIndexEqual(ObjIndex _a, ObjIndex _b)
{
    return (_a.vertex == _b.vertex) &&
           (_a.uv == _b.uv) &&
           (_a.normal == _b.normal);
}

bool LoadOBJ(const char* _path, f32** _vertices, u32** _indices)
{
    if (_path == NULL || _vertices == NULL || _indices == NULL)
    {
        printf("LoadOBJ: invalid argument\n");
        return false;
    }

    FILE* file = fopen(_path, "r");
    if (file == NULL)
    {
        printf("Can not open model: %s\n", _path);
        return false;
    }

    bool loaded = false;

    Vector3* tempVertices = vec_init(256, sizeof(Vector3));
    Vector2* tempUVs = vec_init(256, sizeof(Vector2));
    Vector3* tempNormals = vec_init(256, sizeof(Vector3));
    ObjIndex* faceIndices = vec_init(256, sizeof(ObjIndex));
    ObjIndex* uniqueKeys = vec_init(256, sizeof(ObjIndex));

    f32* outVertices = vec_init(512, sizeof(f32));
    u32* outIndices = vec_init(256, sizeof(u32));

    if (tempVertices == NULL || tempUVs == NULL || tempNormals == NULL ||
        faceIndices == NULL || uniqueKeys == NULL ||
        outVertices == NULL || outIndices == NULL)
    {
        printf("LoadOBJ: out of memory\n");
        goto cleanup;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strncmp(line, "v ", 2) == 0)
        {
            Vector3 vertex = {0};
            if (sscanf(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z) == 3)
            {
                vec_add(&tempVertices, &vertex);
            }
            continue;
        }

        if (strncmp(line, "vt ", 3) == 0)
        {
            Vector2 uv = {0};
            if (sscanf(line + 3, "%f %f", &uv.x, &uv.y) == 2)
            {
                //uv.y = -uv.y;
                vec_add(&tempUVs, &uv);
            }
            continue;
        }

        if (strncmp(line, "vn ", 3) == 0)
        {
            Vector3 normal = {0};
            if (sscanf(line + 3, "%f %f %f", &normal.x, &normal.y, &normal.z) == 3)
            {
                vec_add(&tempNormals, &normal);
            }
            continue;
        }

        if (strncmp(line, "f ", 2) == 0)
        {
            ObjIndex tri[3] = {0};
            char trailing[8] = {0};
            int matches = sscanf(
                line + 2,
                "%u/%u/%u %u/%u/%u %u/%u/%u%7s",
                &tri[0].vertex, &tri[0].uv, &tri[0].normal,
                &tri[1].vertex, &tri[1].uv, &tri[1].normal,
                &tri[2].vertex, &tri[2].uv, &tri[2].normal,
                trailing
            );

            if (matches != 9)
            {
                printf("File can't be read by this parser (expected triangle faces v/vt/vn): %s\n", _path);
                goto cleanup;
            }

            vec_add(&faceIndices, &tri[0]);
            vec_add(&faceIndices, &tri[1]);
            vec_add(&faceIndices, &tri[2]);
            continue;
        }
    }

    for (u32 i = 0; i < vec_count(&faceIndices); i++)
    {
        ObjIndex key = faceIndices[i];
        bool found = false;

        for (u32 v = 0; v < vec_count(&uniqueKeys); v++)
        {
            if (ObjIndexEqual(key, uniqueKeys[v]))
            {
                vec_add(&outIndices, &v);
                found = true;
                break;
            }
        }

        if (!found)
        {
            if (key.vertex == 0 || key.vertex > vec_count(&tempVertices) ||
                key.uv == 0 || key.uv > vec_count(&tempUVs) ||
                key.normal == 0 || key.normal > vec_count(&tempNormals))
            {
                printf("OBJ index out of range while loading: %s\n", _path);
                goto cleanup;
            }

            Vector3 position = tempVertices[key.vertex - 1];
            Vector2 uv = tempUVs[key.uv - 1];

            f32 packed[5] = { position.x, position.y, position.z, uv.x, uv.y };
            vec_append(&outVertices, packed, 5);
            vec_add(&uniqueKeys, &key);

            u32 index = vec_count(&uniqueKeys) - 1;
            vec_add(&outIndices, &index);
        }
    }

    if (vec_count(&outVertices) == 0)
    {
        printf("Warning: Model file %s was empty or had no valid faces\n", _path);
        goto cleanup;
    }

    *_vertices = outVertices;
    *_indices = outIndices;

    outVertices = NULL;
    outIndices = NULL;

cleanup:
    fclose(file);

    if (tempVertices != NULL)
        vec_free(&tempVertices);
    if (tempUVs != NULL)
        vec_free(&tempUVs);
    if (tempNormals != NULL)
        vec_free(&tempNormals);
    if (faceIndices != NULL)
        vec_free(&faceIndices);
    if (uniqueKeys != NULL)
        vec_free(&uniqueKeys);
    if (outVertices != NULL)
        vec_free(&outVertices);
    if (outIndices != NULL)
        vec_free(&outIndices);

    return true;
}

Model BuildModel(f32** _vertices, u32** _indices, DrawInfo _info)
{
    Model model;
    model.vertices = *_vertices;
    model.indices = *_indices;

    _vertices = NULL;
    _indices = NULL;

    glGenVertexArrays(1, &model.VAO);
    glGenBuffers(1, &model.VBO);
    glGenBuffers(1, &model.EBO);

    glBindVertexArray(model.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, model.VBO);
    glBufferData(GL_ARRAY_BUFFER, vec_size_of((void*)&model.vertices), model.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vec_size_of((void*)&model.indices), model.indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return model;
}

void DrawModel(Model _model)
{
    glBindVertexArray(_model.VAO);
    glDrawElements(GL_TRIANGLES, vec_count(&_model.indices), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void FreeModel(Model _model)
{
    glDeleteVertexArrays(1, &_model.VAO);
    glDeleteBuffers(1, &_model.VBO);
    glDeleteBuffers(1, &_model.EBO);
    vec_free(&_model.vertices);
    vec_free(&_model.indices);
}
