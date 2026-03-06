#pragma once

#define PROGRESS_BAR_WIDTH 25
#define CHUNK_SIZE 4096

#include "queue.h"
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>

typedef struct {
    char data[CHUNK_SIZE]; 
    size_t len;               
    bool is_last;              
} chunk_t;

extern volatile sig_atomic_t keep_running;

typedef struct {
    FILE* source_file;
    size_t src_size;
    FILE* destination_file;
    queue* q;
} thread_args_t;

void* reader_thread(void* arg);
void* writer_thread(void* arg);