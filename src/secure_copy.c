#include "secure_copy.h"
#include "caesar.h"
#include <string.h>



void* reader_thread(void* arg) {
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

        //  pass address of the read data
        enqueue(args->q, &chunk);
        
        if (chunk->is_last) 
            break;
    }
    return NULL;
}

void* writer_thread(void* arg) {
    thread_args_t* args = (thread_args_t*)arg;
    chunk_t *chunk;
    while (true) {
        if (get_size(args->q) == 0) {
            sched_yield();
            continue;
        }
        
        dequeue(args->q, &chunk);
        
        unsigned char processed[CHUNK_SIZE];
        caesar(chunk->data, processed, chunk->len);
        fwrite(processed, 1, chunk->len, args->destination_file);
        
        if (chunk->is_last) {
            free(chunk);
            break; 
        }
        free(chunk);  
    }
    return NULL;
}