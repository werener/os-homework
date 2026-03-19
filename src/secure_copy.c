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
    char *src_path_TEMP = args->src_names[0];
    char *dest_path_TEMP = args->dest_name;

    FILE* src_file = fopen(src_path_TEMP, "rb");
    if (!src_file) {
        printf("Couldn't open file '%s'\n", src_path_TEMP);
        return NULL;
    }
    FILE* dest_file = fopen(dest_path_TEMP, "wb");
    if (!dest_file) {
        printf("Couldn't open file '%s'\n", dest_path_TEMP);
        fclose(src_file);
        return NULL;
    }
    process_file(src_file, dest_file);    

    return NULL;
}