#include "array.h"

#include <string.h>
#include <stdio.h>

array_t *array_init() {
	array_t *arr = malloc(sizeof(array_t));
    arr->count = 0;
    arr->capacity = 16;
    arr->data = malloc(arr->capacity * sizeof(char *));
    if (!arr->data) {
        fprintf(stderr, "Couldn't allocate array for files\n");
		free(arr);
		return NULL;
    }
	return arr;
}

int array_push(array_t *arr, const char *filename) {
    // check whether we need to realloc
    if (arr->count >= arr->capacity) {
		arr->capacity *= 2;

        char **new_data = realloc(arr->data, arr->capacity * sizeof(char *));
        if (!new_data) {
            fprintf(stderr, "Couldn't reallocate array for files\n");
            return -1;
        }
        arr->data = new_data;
    }

    // add the provided file to the array
    size_t len = strlen(filename);
    arr->data[arr->count] = malloc(len + 1);

    memcpy(arr->data[arr->count], filename, len + 1);

    arr->count++;
    return 0;
}

void array_free(array_t *arr) {
    if (!arr) return;
    for (size_t i = 0; i < arr->count; ++i) {
        free(arr->data[i]);
    }
    free(arr->data);
	free(arr);
	arr = NULL;
}
