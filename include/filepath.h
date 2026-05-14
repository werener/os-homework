#pragma once
#include <libgen.h>
#include <sys/stat.h>

int is_directory(const char *path);
char *make_copy_target(char *source_file, char *out_folder);
