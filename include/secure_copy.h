#pragma once

#define PROGRESS_BAR_WIDTH 25
#define BUFFER_SIZE 4096

#include "queue.h"
#include <pthread.h>

typedef struct {
    char **src_names;
    char *dest_name;
} thread_args_t;

void *worker(void* arg);
void process_file(FILE *src_file, FILE *dest_file);