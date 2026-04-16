#include "caesar.h"
#include "logging.h"
#include "secure_copy.h"
#include "string.h"
#include <bits/getopt_core.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#define WORKER_COUNT 4

#define MODE_AUTO -1
#define MODE_SEQUENTIAL 0
#define MODE_PARALLEL 1

int main(int argc, char **argv) {
    const char *bin_title = argv[0];
    int opt;
    int option_index = 0;

    /* Option parsing */
    int mode = MODE_AUTO;
    static struct option long_options[] = {
        {"mode", optional_argument, 0, 'm'},
        {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, (char *const *)argv, "m:", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'm':
            if (optarg[0] == '=')
                optarg += 1;
            if (strcmp(optarg, "sequential") == 0) {
                mode = MODE_SEQUENTIAL;
                break;
            }
            if (strcmp(optarg, "parallel") == 0) {
                mode = MODE_PARALLEL;
                break;
            }
            fprintf(stderr, "No mod '%s' availiable\n", optarg);
            /* fallthrough */
        default:
            fprintf(stderr, "Usecase example: %s <FILE> [FILES...] <COPY_DIR> <KEY>\n", bin_title);
            return 1;
        }
    }

    /* Argument parsing */
    int remaining_args = argc - optind;
    if (remaining_args < 3) {
        fprintf(stderr, "Usecase example: %s <FILE> [FILES...] <COPY_DIR> <KEY>\n", bin_title);
        return 1;
    }

    int num_sources = remaining_args - 2;

    args_t args = {
        .src_names = &argv[optind],
        .dest_name = argv[argc - 2],
        .total_sources = num_sources,
        .sources_processed = 0,
    };

    if (mode == MODE_AUTO)
        mode = num_sources <= 4 ? MODE_SEQUENTIAL : MODE_PARALLEL;

    /* Validate key */
    char *key_arg = argv[argc - 1];
    if (strlen(key_arg) != 1) {
        fprintf(stderr, "Key should be exactly 1 byte - '%s' isn't\n", key_arg);
        return 1;
    }
    caesar_key(key_arg[0]);

    /* Setup logging */
    log_file = fopen("log.log", "a");
    if (!log_file) {
        perror("Failed to create log.txt");
        return 1;
    }

    /* Start execution */
    struct timespec start, end;
    double execution_time;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (mode == MODE_PARALLEL) {
        log_custom_message("\tStarted logging parallel execution\n");
        pthread_t pool[WORKER_COUNT];
        for (int i = 0; i < WORKER_COUNT; ++i) {
            pthread_create(&pool[i], NULL, worker, &args);
        }
        for (int i = 0; i < WORKER_COUNT; ++i) {
            pthread_join(pool[i], NULL);
        }
    } 
    else if (mode == MODE_SEQUENTIAL) {
        log_custom_message("\tStarted logging sequential execution\n");
        sequential(args);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    execution_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time: %f seconds\n", execution_time);

    log_custom_message("\tFinished logging\n\n");
    close_log();
    return 0;
}