#include "caesar.h"
#include "secure_copy.h"
#include "string.h"


int main(int argc, char **argv) {
    /* (1) handle arguments */
    if (argc < 4) {
        char *bin_title = argv[0];
        printf("Usecase example: %s <FILE> [FILES...] <COPY_DIR> <KEY>\n", bin_title);
        return 1;
    }
    
    int num_sources = argc - 3;
    char *src_path = &argv[1];

    char *destination_path = argv[argc - 2];
    char *key_arg = argv[argc - 1];

    /* (2) validate key */
    if (strlen(key_arg) != 1) {
        fprintf(stderr, "Key should be exactly 1 byte - '%s' isn't\n", key_arg);
        return 1;
    }
    caesar_key(key_arg[0]);
    

    /* (6) setup threads */
    thread_args_t args = { 
        .src_names = src_path, 
        .dest_name = destination_path, 
    };
    pthread_t thread_reader, thread_writer;
    pthread_create(&thread_reader, NULL, worker, &args);

    pthread_join(thread_reader, NULL);
    pthread_join(thread_writer, NULL);

    // free the memory
    return 0;
}