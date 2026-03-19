#pragma once

#define PROGRESS_BAR_WIDTH 25
#define BUFFER_SIZE 4096

#include "queue.h"
#include "logging.h"
#include "filepath.h"
#include <pthread.h>
#include <sys/time.h>

typedef struct {
    char **src_names;
    char *dest_name;
    int total_sources;
    int sources_processed;
} thread_args_t;

void *worker(void* arg);
void process_file(FILE *src_file, FILE *dest_file, const char *filename);