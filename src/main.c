#include "caesar.h"
#include "logging.h"
#include "secure_copy.h"
#include "string.h"

#include <getopt.h>
#include <signal.h>

#define WORKER_COUNT 4

#define MODE_AUTO -1
#define MODE_SEQUENTIAL 0
#define MODE_PARALLEL 1

void segv_handler(int);
void sigint_handler(int);
double run_parallel(args_t);
double run_sequential(args_t);
void print_execution_time(double execution_time, args_t);

int main(int argc, char **argv) {
    signal(SIGSEGV, segv_handler);
    signal(SIGINT, sigint_handler);

    int EXIT_CODE = 0;
    const char *bin_title = argv[0];

    /* Option parsing */
    int opt;
    int option_index = 0;
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
            fprintf(stderr, "No mode '%s' availiable\n", optarg);
            /* fallthrough */
        default:
            fprintf(stderr, "Usecase example: %s <FILE> [FILES...] <COPY_DIR> <KEY>\n", bin_title);
            EXIT_CODE = 1;
            goto CLEANUP;
        }
    }

    /* Argument parsing */
    int remaining_args = argc - optind;
    if (remaining_args < 3) {
        fprintf(stderr, "Usecase example: %s <FILE> [FILES...] <COPY_DIR> <KEY>\n", bin_title);
        EXIT_CODE = 1;
        goto CLEANUP;
    }

    int num_sources = remaining_args - 2;

    args_t args = {
        .src_names = &argv[optind],
        .dest_name = argv[argc - 2],
        .total_sources = num_sources,
        .sources_processed = 0,
    };

    /* Validate key */
    char *key_arg = argv[argc - 1];
    if (strlen(key_arg) != 1) {
        fprintf(stderr, "Key should be exactly 1 byte - '%s' isn't\n", key_arg);
        EXIT_CODE = 1;
        goto CLEANUP;
    }
    if (caesar_key(key_arg[0]))
        goto CLEANUP;

    /* Setup logging */
    const char *LOGFILE = "log.log";
    log_file = fopen(LOGFILE, "a");
    if (!log_file) {
        fprintf(stderr, "Failed to create %s", LOGFILE);
        goto CLEANUP;
    }

    /* Start execution */
    double execution_time;    

    if (mode == MODE_PARALLEL) {
        execution_time = run_parallel(args);
        print_execution_time(execution_time, args);
    } else if (mode == MODE_SEQUENTIAL) {
        execution_time = run_sequential(args);
        print_execution_time(execution_time, args);
    } else {
        double execution_time_parallel = run_parallel(args);
        double execution_time_sequential = run_sequential(args);

        printf("\tParallel:\n");
        print_execution_time(execution_time_parallel, args);
        printf("\tSequential:\n");
        print_execution_time(execution_time_sequential, args);
    }

CLEANUP:
    free_page();
    close_log();
    return EXIT_CODE;
}

double run_parallel(args_t args) {
    printf("\nRunning in parallel mode\n");
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    log_custom_message("\tStarted logging parallel execution\n");
    pthread_t pool[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; ++i)
        pthread_create(&pool[i], NULL, worker, &args);

    for (int i = 0; i < WORKER_COUNT; ++i)
        pthread_join(pool[i], NULL);
    log_custom_message("\tFinished logging\n\n");

    clock_gettime(CLOCK_MONOTONIC, &end);
    
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

double run_sequential(args_t args) {
    printf("\nRunning in sequential mode\n");
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    log_custom_message("\tStarted logging sequential execution\n");
    sequential(args);
    log_custom_message("\tFinished logging\n\n");

    clock_gettime(CLOCK_MONOTONIC, &end);
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

void print_execution_time(double execution_time, args_t args) {
    printf("Execution time: %f seconds\n", execution_time);
    printf("Average time per file: %f seconds\n", execution_time / args.total_sources);
}

void segv_handler(int sig) {
    INTERRUPTION = sig;
    fprintf(stderr, "%d: Read of protected memory detected\n", sig);
    log_custom_message("\tExited programm with segmentation fault\n\n");
    _exit(sig);
}

void sigint_handler(int sig) {
    INTERRUPTION = sig;
    fprintf(stderr, "%d: Keyboard interruption\n", sig);
    log_custom_message("\tProgram stopped via keyboard interruption\n\n");
    _exit(sig);
}
