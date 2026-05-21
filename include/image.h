#pragma once

#include <stdint.h>
#include <stdio.h>

#define SALT_SIZE 16
#define IMAGE_ERROR -1


typedef struct __attribute__((packed)) {
    int32_t data_len;     // 4
    int32_t name_len;     // 4
    char salt[SALT_SIZE]; // 16
    char *name;           // 1
    char *data;           // 1
} file_t;                 // 26

typedef struct __attribute__((packed)) {
    file_t *files;        // 1
    int32_t files_amount; // 4
} image_t;                // 5

int count_files(char *path);
image_t *get_image(char *path);

void print_file(file_t file);