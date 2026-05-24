#pragma once

#include <stdint.h>
#include <stdio.h>

#define SALT_SIZE 16
#define IMAGE_ERROR -1

typedef struct __attribute__((packed)) {
    int32_t data_len;     // 4
    int32_t name_len;     // 4
    char salt[SALT_SIZE]; // 16
    char *name;           // 8
    char *data;           // 8
} file_t;                 // 40

typedef struct __attribute__((packed)) {
    file_t *files;        // 8
    int32_t files_amount; // 4
} image_t;                // 12

void print_file(file_t file);

image_t *get_image(char *path);

void sort_image(image_t *image);

file_t *get_by_name(image_t *image, char *name);