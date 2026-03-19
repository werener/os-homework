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
    char **sources = &argv[1];

    char *destination_folder = argv[argc - 2];
    char *key_arg = argv[argc - 1];

    /* (2) validate key */
    if (strlen(key_arg) != 1) {
        fprintf(stderr, "Key should be exactly 1 byte - '%s' isn't\n", key_arg);
        return 1;
    }
    caesar_key(key_arg[0]);
    

    /* (6) setup threads */
    thread_args_t args = { 
        .src_names = sources, 
        .dest_name = destination_folder, 
        .total_sources = num_sources,
        .sources_processed = 0,
    };
    pthread_t thread1, thread2; 
    pthread_create(&thread1, NULL, worker, &args);
    pthread_create(&thread2, NULL, worker, &args);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // free the memory
    return 0;
}