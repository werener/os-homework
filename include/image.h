#pragma once

#include "rc4.h"
#include "filepath.h"

#include <stdint.h>
#include <stdio.h>

#define SALT_SIZE 16
#define IMAGE_ERROR -1

typedef struct __attribute__((packed)) {
	int32_t data_len;
	int32_t name_len;
	char salt[SALT_SIZE];
} metadata_t;

// typedef struct {

// } args_t;

int count_files(const char *path);