#include "caesar.h"
#include "logging.h"
#include "secure_copy.h"
#include "string.h"
#include <bits/getopt_core.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#define WORKER_COUNT 4

#define mode_auto -1
#define mode_sequential 0
#define mode_parallel 1



int main(int argc, char **argv) {
    const char *bin_title = argv[0];
    int opt;
    int option_index = 0;
    
    
    int mode = mode_auto;
    static struct option long_options[] = {
        {"mode", optional_argument, 0, 'm'},  // --mode=value or --mode value
        {0, 0, 0, 0}  // Terminator entry - must be all zeros
    };

    while ((opt = getopt_long(argc, (char * const *)argv, "m:", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'm':
            if (optarg[0] == '=')
                optarg += 1;
            if (strcmp(optarg, "sequential") == 0) {
                mode = mode_sequential;
                break; 
            }
            if (strcmp(optarg, "parallel") == 0) {
                mode = mode_parallel;
                break; 
            }
            printf("No mod '%s' availiable\n", optarg);
        default:
            fprintf(stderr, "Usecase example: %s <FILE> [FILES...] <COPY_DIR> <KEY>\n", bin_title);
            return 1;
        }
    }

    int remaining_args = argc - optind;

    // printf("%d, %d: %s\n", optind, remaining_args, argv[optind]);
     if (remaining_args < 3) {
        printf("Usecase example: %s <FILE> [FILES...] <COPY_DIR> <KEY>\n", bin_title);
        return 1;
    }
    
    int num_sources = remaining_args - 2;
    char **sources = &argv[optind];

    char *destination_folder = argv[argc - 2];
    char *key_arg = argv[argc - 1];

    if (mode == mode_auto) {
        if (num_sources <= 4) 
            mode = mode_sequential;
        else
            mode = mode_parallel;
    }
   


    /* (2) validate key */
    if (strlen(key_arg) != 1) {
        fprintf(stderr, "Key should be exactly 1 byte - '%s' isn't\n", key_arg);
        return 1;
    }
    caesar_key(key_arg[0]);
    

    return 0;
    /* (3) setup threads */
    thread_args_t args = { 
        .src_names = sources, 
        .dest_name = destination_folder, 
        .total_sources = num_sources,
        .sources_processed = 0,
    };
    /* setup logging */
   
    log_file = fopen("log.txt", "a");
    if (!log_file) {
        perror("Failed to create log.txt");
        return 1;
    }

    log_custom_message("\tStarted logging\n");

    pthread_t pool[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; ++i) {
        pthread_create(&pool[i], NULL, worker, &args);
    }
    for (int i = 0; i < WORKER_COUNT; ++i) {
        pthread_join(pool[i], NULL);;
    }

    // free the memory
    log_custom_message("\tFinished logging\n\n");
    close_log();

    
    return 0;

}