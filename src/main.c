#include "caesar.h"
#include "secure_copy.h"
#include "string.h"
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    (void) sig;
    keep_running = 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        char *bin_title = argv[0];
        printf("Usecase example: %s <path_to_src> <path_to_target> <key>\n", bin_title);
        return 1;
    }
    
    char *src_path = argv[1];
    char *destination_path = argv[2];
    char *key_arg = argv[3];

    signal(SIGINT, handle_sigint);

    /* (2) validate key */
    if (strlen(key_arg) != 1) {
        fprintf(stderr, "Key should be exactly 1 byte - '%s' isn't\n", key_arg);
        return 1;
    }
    caesar_key(key_arg[0]);

    /* (3) setup queue */
    queue *q = create_queue(sizeof(chunk_t*));
    q->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(q->mutex, NULL);

    /* (4) open source_file */
    FILE* src_file = fopen(src_path, "rb");
    if (!src_file) {
        printf("Couldn't open file '%s'\n", src_path);
        destroy_queue(q);
        return 1;
    }
    fseek(src_file, 0, SEEK_END);
    size_t src_size = ftell(src_file);
    rewind(src_file);

    /* (5) open destination_file */
    
    FILE* destination_check = fopen(destination_path, "rb");
    bool destination_existed = false;
    if (destination_check) {
        destination_existed = true;
        fclose(destination_check);
    }

    FILE* dest_file = fopen(destination_path, "wb");
    if (!dest_file) {
        printf("Couldn't open file '%s'\n", destination_path);
        fclose(src_file);
        destroy_queue(q);
        return 1;
    }
   
    /* (6) setup threads */
    thread_args_t args = { 
        .source_file = src_file, 
        .destination_file = dest_file, 
        .q = q,
        .src_size = src_size
    };
    pthread_t thread_reader, thread_writer;
    pthread_create(&thread_reader, NULL, reader_thread, &args);
    pthread_create(&thread_writer, NULL, writer_thread, &args);

    pthread_join(thread_reader, NULL);
    pthread_join(thread_writer, NULL);
   
    // free the memory
    fclose(dest_file);
    fclose(src_file);
    destroy_queue(q);

    if (!keep_running) {
        printf("\n %s wasn't copied fully due to interruption  \n", destination_path);
        if (!destination_existed) { 
            unlink(destination_path);
        }
        return 1;
    }

    return 0;
}