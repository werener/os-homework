#pragma once

#include <stdint.h>


typedef struct {
    int8_t salt[16];
    int32_t data_len;
    int32_t name_len;
    char *name;
    int8_t *data;
} file_t;

typedef struct {
    file_t *files;
} container_t;

