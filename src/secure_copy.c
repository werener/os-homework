#include "secure_copy.h"
#include "caesar.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>


pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
void process_file(FILE *src_file, FILE *dest_file, const char *filename) {
    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Failed to allocate memory");
        write_to_log(filename, "ERROR: Memory allocation failed");
        return;
    }   
    char *processed = malloc(BUFFER_SIZE);
    if (!processed) {
        free(buffer);
        perror("Failed to allocate memory");
        write_to_log(filename, "ERROR: Memory allocation failed");
        return;
    }
    write_to_log(filename, "Opened for read");
    while (true) {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, src_file);
        caesar(buffer, processed, BUFFER_SIZE);
        size_t bytes_written = fwrite(processed, 1, bytes_read, dest_file);

        if (bytes_written != BUFFER_SIZE)
            break;
    }

    write_to_log(filename, "Finished processing");

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
            printf("Possible deadlock\n");
            write_to_log("-", "ERROR: Deadlock");
            break;
        }
        
        if (args->sources_processed >= args->total_sources) {
            pthread_mutex_unlock(&counter_mutex);
            break;
        }
        
        int currently_processing = args->sources_processed;
        args->sources_processed++;
        char *filename = args->src_names[currently_processing];

        if (is_directory(filename)) {
            printf("(%i/%i) %s is a folder. Can't process it.\n",  currently_processing, args->total_sources, filename);            // continue; // FOR ARTIFICIAL DEADLOCK
            pthread_mutex_unlock(&counter_mutex);
            continue;
        }

        fprintf(stderr, "(%i/%i) Processing '%s'.\n", currently_processing, args->total_sources, filename);
        pthread_mutex_unlock(&counter_mutex);
        
        char *destination_folder = args->dest_name;
        char *fullpath = make_copy_target(filename, destination_folder);
        
        FILE* src_file = fopen(filename, "rb");
        if (!src_file) {
            fprintf(stderr, "Couldn't open file '%s'\n", filename);
            write_to_log(filename, "ERROR: Couldn't open source file");
            free(fullpath);
            continue;
        }
        FILE* dest_file = fopen(fullpath, "wb");
        if (!dest_file) {
            printf("Couldn't open file '%s' (Possibly no folder '%s')\n", fullpath, destination_folder);
            write_to_log(fullpath, "ERROR: Couldn't open destination file");
            fclose(src_file);
            free(fullpath);
            continue;
        }
        
        process_file(src_file, dest_file, filename);
        fclose(src_file);
        fclose(dest_file);
        free(fullpath);
    }

    return NULL;
}