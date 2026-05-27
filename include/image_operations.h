#pragma once
#include "image.h"

void list(const char *img_path);

void get(const char *img_path, byte *key, const char *file, const char *out);

void add(const char *img_path, byte *key, char **files, int file_amount);