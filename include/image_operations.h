#pragma once
#include "image.h"

int list(const char *img_path);

int get(const char *img_path, byte *key, const char *file, const char *out);

int add(const char *img_path, byte *key, char **files, int file_amount);