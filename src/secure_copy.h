#pragma once
#include <pthread.h>
#include "queue.h"
#include <sched.h>
#include <time.h>

#define CHUNK_SIZE 4096

typedef struct {
    char data[CHUNK_SIZE]; 
    size_t len;               
    bool is_last;              
} chunk_t;

typedef struct {
    FILE* source_file;
    size_t src_size;
    FILE* destination_file;
    queue* q;
} thread_args_t;

void* reader_thread(void* arg);
void* writer_thread(void* arg);