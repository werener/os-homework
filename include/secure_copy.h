#pragma once

#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>

#define DEADLOCK_DETECTION_TIMER_SEC 5
#define BUFFER_SIZE 4096

typedef struct {
    char **src_names;
    char *dest_name;
    int total_sources;
    int sources_processed;
} args_t;

void *worker(void *arg);
void process_file(FILE *src_file, FILE *dest_file, const char *filename);
void sequential(args_t args);