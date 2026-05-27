#pragma once

#include <stdlib.h>

typedef struct {
    char **data;
    size_t count;
    size_t capacity;
} array_t;

array_t *array_init(void);
int array_push(array_t *arr, const char *str);
void array_free(array_t *arr);