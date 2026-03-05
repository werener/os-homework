#include "secure_copy.h"
#include "caesar.h"
#include <string.h>

void draw_progress_bar(size_t total_bytes_written, size_t src_size) {
    printf("\r[");
    int percentage = (total_bytes_written * 100) / src_size;
    int pos = PROGRESS_BAR_WIDTH * percentage / 100;
    for (int i = 0; i < PROGRESS_BAR_WIDTH; i++) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %d%% (%zu/%zu bytes)", 
            percentage, total_bytes_written, src_size);
    fflush(stdout);
}

void *reader_thread(void* arg) {
    thread_args_t *args = (thread_args_t*) arg;
    while (true) {
        chunk_t *chunk = malloc(sizeof(chunk_t));
        chunk->len = fread(
            chunk->data, 
            1,
            sizeof(chunk->data), 
            args->source_file
        );
        /* 
        indicate to the other thread:
        some bytes were not read correctly  
        */
        chunk->is_last = (chunk->len < sizeof(chunk->data));

        enqueue(args->q, &chunk);
        
        if (chunk->is_last) 
            break;
    }
    return NULL;
}

void* writer_thread(void* arg) {
    thread_args_t* args = (thread_args_t*)arg;
    chunk_t *chunk;
    char processed[CHUNK_SIZE];


    size_t total_bytes_written = 0;
    struct timespec last_update_time;
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &last_update_time);
    draw_progress_bar(total_bytes_written, args->src_size);

    while (true) {
        if (get_size(args->q) == 0) {
            sched_yield();
            continue;
        }
        
        dequeue(args->q, &chunk);
        caesar(chunk->data, processed, chunk->len);
        fwrite(processed, 1, chunk->len, args->destination_file);
        
        total_bytes_written += chunk->len;
        
        
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        long elapsed_ms = (current_time.tv_sec - last_update_time.tv_sec) * 1000 +
                         (current_time.tv_nsec - last_update_time.tv_nsec) / 1000000;
        
        if ((elapsed_ms >= 100) || chunk->is_last) {
            draw_progress_bar(total_bytes_written, args->src_size);
            last_update_time = current_time;
        }
        
        if (chunk->is_last) {
            printf("\n");
            free(chunk);
            break; 
        }
        free(chunk);  
    }
    return NULL;
}