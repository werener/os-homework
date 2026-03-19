#include "secure_copy.h"
#include "caesar.h"
#include <string.h>
#include <stdlib.h>

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
    process_file(args->source_file, args->destination_file);    
}