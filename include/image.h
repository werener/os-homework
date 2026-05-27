#pragma once

#include "filepath.h"
#include "rc4.h"

#include <stdint.h>
#include <stdio.h>

#define BUFFER_SIZE 8192
#define SALT_SIZE 16
#define DEADLOCK_DETECTION_TIMER_SEC 5
#define IMAGE_ERROR -1

typedef struct __attribute__((packed)) {
    int32_t data_len;
    int32_t name_len;
    char salt[SALT_SIZE];
} metadata_t;

typedef struct {
    array_t *files;
    size_t processed;
	byte *key;
	size_t len_key;
	FILE *img_f;
} add_args_t;

int count_files(const char *path);
byte *add_salt(char salt[SALT_SIZE], byte *key, size_t len_key);

void *add_worker(void *arg);