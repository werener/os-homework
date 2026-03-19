#include "secure_copy.h"
#include "filepath.h"
#include "caesar.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void process_file(FILE *src_file, FILE *dest_file) {
    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Failed to allocate memory");
        return;
    }   
    char *processed = malloc(BUFFER_SIZE);
    if (!processed) {
        free(buffer);
        perror("Failed to allocate memory");
        return;
    }
    while (true) {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, src_file);
        caesar(buffer, processed, BUFFER_SIZE);
        size_t bytes_written = fwrite(processed, 1, bytes_read, dest_file);

        if (bytes_written != BUFFER_SIZE)
            break;
    }

    free(buffer);
    free(processed);
    return;
}

void *worker(void* arg) {
    thread_args_t *args = (thread_args_t*) arg;
    
    while (true) {
        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 5;
        
        int lock_result = pthread_mutex_timedlock(&counter_mutex, &timeout);
        if (lock_result == ETIMEDOUT) {
            printf("Possible deadlock: mutex is being awaited for 5 seconds by process %s\n", "PROCESS_ID");
            continue;
        }
        
        if (args->sources_processed >= args->total_sources) {
            pthread_mutex_unlock(&counter_mutex);
            break;
        }
        
        char *src_path_TEMP = args->src_names[args->sources_processed];
        args->sources_processed++;
        pthread_mutex_unlock(&counter_mutex);
        
        char *dest_path_TEMP = args->dest_name;
        char *fullpath = make_copy_target(src_path_TEMP, dest_path_TEMP);
        printf("HERE: %s\n\n", fullpath);

        FILE* src_file = fopen(src_path_TEMP, "rb");
        if (!src_file) {
            fprintf(stderr, "Couldn't open file '%s'\n", src_path_TEMP);
            free(fullpath);
            continue;
        }
        FILE* dest_file = fopen(fullpath, "wb");
        if (!dest_file) {
            printf("Couldn't open file '%s' (Possibly no folder '%s')\n", fullpath, dest_path_TEMP);
            fclose(src_file);
            free(fullpath);
            continue;
        }
        process_file(src_file, dest_file);
        
        fclose(src_file);
        fclose(dest_file);
        free(fullpath);
    }

    return NULL;
}