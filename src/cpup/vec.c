#include "vec.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned int capacity;
    unsigned int count;
    size_t elementSize;
} vec_info;

void vec_init(void* _refVec, unsigned int _capacity, size_t _elementSize) {
    vec_info* info = malloc(sizeof(vec_info) + (_capacity * _elementSize));

    if (info == NULL) {
        printf("Failed to init vector!");
        exit(1);
    }

    *(void**)_refVec = info + 1;

    info->capacity = _capacity;
    info->count = 0;
    info->elementSize = _elementSize;
}

void vec_free(void* _refList) {
    vec_info* info = *(vec_info**)_refList - 1;

    free(info);
    *((void**)_refList) = NULL;
}

void vec_clear(void* _refList) {
    vec_info* info = *(vec_info**)_refList - 1;

    info->count = 0;
}

void vec_add(void* _refVec, void* _value) {
    vec_info* info = *(vec_info**)_refVec - 1;

    // check if vec needs to grow
    if (info->count >= info->capacity) {
        info->capacity = (info->capacity == 0) ? 1 : (info->capacity * 2);

        info = realloc( info, sizeof(vec_info) + (info->capacity * info->elementSize));

        if (info == NULL) {
            printf("Failed to resize vector!");
            exit(1);
        }

        *(void**)_refVec = info + 1;
    }

    memcpy(
        *(char**)_refVec + (info->count * info->elementSize),
        _value,
        info->elementSize
    );

    info->count++;
}

void vec_remove_at(void* _refVec, unsigned int _index) {
    vec_info* info = *(vec_info**)_refVec - 1;

    if (info->count == 0 || _index >= info->count) {
        return;
    }

    void* current = *(char**)_refVec + (_index * info->elementSize);
    void* next = (char*)current + info->elementSize;
    size_t bytes = (size_t)(info->count - _index - 1) * info->elementSize;

    if (bytes > 0) {
        memmove(current, next, bytes);
    }

    info->count--;
}

unsigned int vec_count(void* _refVec) {
    vec_info* info = *(vec_info**)_refVec - 1;

    return info->count;
}

void vec_save(void* _refVec, const char* _file) {
    vec_info* info = *(vec_info**)_refVec - 1;

    FILE *file;

    file = fopen(_file, "wb");

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for writing\n");
        exit(1);
    }

    size_t written = fwrite((void*)info, sizeof(vec_info) + (info->capacity * info->elementSize), 1, file);

    if (written != 1) {
        fprintf(stderr, "Error writing to file (short write)\n");
    } else {
        printf("Successfully wrote 1 record to %s\n", _file);
    }

    fclose(file);
}

void vec_load(void* _refVec, const char* _file) {
    FILE *file;

    file = fopen(_file, "rb");

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for reading\n");
        exit(1);
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error seeking file\n");
        fclose(file);
        exit(1);
    }

    long file_size = ftell(file);
    if (file_size < (long)sizeof(vec_info)) {
        fprintf(stderr, "Error reading file (too small)\n");
        fclose(file);
        exit(1);
    }

    rewind(file);

    vec_info* info = malloc((size_t)file_size);
    if (info == NULL) {
        fprintf(stderr, "Failed to allocate memory for vector load\n");
        fclose(file);
        exit(1);
    }

    size_t read = fread(info, 1, (size_t)file_size, file);

    if (read != (size_t)file_size) {
        fprintf(stderr, "Error reading file (short read)\n");
        free(info);
        fclose(file);
        exit(1);
    }

    size_t expected_size = sizeof(vec_info) + (info->capacity * info->elementSize);
    if ((size_t)file_size < expected_size) {
        fprintf(stderr, "Error reading file (size mismatch)\n");
        free(info);
        fclose(file);
        exit(1);
    }

    if (*(void**)_refVec != NULL) {
        vec_info* old = *(vec_info**)_refVec - 1;
        free(old);
    }

    *(void**)_refVec = info + 1;

    fclose(file);
}
