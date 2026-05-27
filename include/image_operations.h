#pragma once
#include "image.h"

#define BUFFER_SIZE 8192

void list(const char *img_path);

void get(const char *img_path, byte *key, const char *file, const char *out);

void add(const char *img_path, byte *key, const char **files);