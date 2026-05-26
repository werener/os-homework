#pragma once

#include "rc4.h"
#include <stdint.h>
#include <stdio.h>

#define SALT_SIZE 4
#define IMAGE_ERROR -1

typedef struct __attribute__((packed)) {
	int32_t data_len;
	int32_t name_len;
	char salt[SALT_SIZE];
} metadata_t;

int count_files(const char *path);