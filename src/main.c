#include "caesar.h"
#include "secure_copy.h"
#include "string.h"


int main(int argc, char *argv[]) {
    if (argc != 4) {
        char *bin_title = argv[0];
        printf("Usecase example: %s <path_to_src> <path_to_target> <key>\n", bin_title);
        return 1;
    }
    
    char *src_path = argv[1];
    char *destination_path = argv[2];
    char *key_arg = argv[3];

    /* (2) validate key */
    if (strlen(key_arg) != 1) {
        fprintf(stderr, "Key should be exactly 1 byte - '%s' isn't\n", key_arg);
        return 1;
    }
    caesar_key(key_arg[0]);
    
    /* (3) open source_file */
    FILE* src_file = fopen(src_path, "rb");
    if (!src_file) 
        printf("Couldn't open file '%s'\n", src_path);

    /* (4) open destination_file */
    FILE* dest_file = fopen(destination_path, "wb");
    if (!dest_file) {
        printf("Couldn't open file '%s'\n", destination_path);
        fclose(src_file);
    }

    /* (5) setup queue */
    queue *q = create_queue(sizeof(chunk_t));
    q->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(q->mutex, NULL);

    /* (6) setup threads */
    thread_args_t args = { 
        .source_file = src_file, 
        .destination_file = dest_file, 
        .q = q 
    };
    pthread_t thread_reader, thread_writer;
    pthread_create(&thread_reader, NULL, reader_thread, &args);
    pthread_create(&thread_writer, NULL, writer_thread, &args);

    
    // wait for the threads to end
    pthread_join(thread_reader, NULL);
    pthread_join(thread_writer, NULL);
    destroy_queue(q); 

    fclose(src_file);
    fclose(dest_file);
    return 0;
}