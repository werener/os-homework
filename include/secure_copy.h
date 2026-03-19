#pragma once

#define PROGRESS_BAR_WIDTH 25
#define BUFFER_SIZE 4096

#include "queue.h"
#include <pthread.h>

typedef struct {
    FILE* source_file;
    size_t src_size;
    FILE* destination_file;
    queue* q;
} thread_args_t;

void *worker(void* arg);
void process_file(FILE *src_file, FILE *dest_file);