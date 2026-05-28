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
	uint32_t data_len;
	uint32_t name_len;
	byte salt[SALT_SIZE];
} metadata_t;

typedef struct {
	array_t *files;
	size_t processed;
	byte *key;
	size_t len_key;
	int img_fd;
	size_t curr_offset;
} add_args_t;

extern pthread_mutex_t fetch_file_mx;
extern pthread_mutex_t image_mx;

int count_files(const char *path);
byte *add_salt(byte salt[SALT_SIZE], byte *key, size_t len_key);

void *add_worker(void *arg);