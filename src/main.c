#include "args.h"
#include "caesar.h"
#include "logging.h"
#include "secure_copy.h"
#include "string.h"

#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define WORKER_COUNT 4
#define MODE_SEQUENTIAL 0
#define MODE_PARALLEL 1

#define KEY_OPT 'k'
#define IMG_OPT 'i'
#define OUT_OPT 'o'

void segv_handler(int);
void sigint_handler(int);
void cleanup();

int main(int argc, char **argv) {
    signal(SIGSEGV, segv_handler);
    signal(SIGINT, sigint_handler);

    cli_args_t args = {
        .bin_title = argv[0],
        .key = NULL,
        .image = NULL,
        .out = NULL,
        .command = CMD_NONE,
        .files = NULL,
    };

    static struct option long_options[] = {
        {"key", required_argument, 0, KEY_OPT},
        {"image", required_argument, 0, IMG_OPT},
        {"out", required_argument, 0, OUT_OPT},
        {"help", optional_argument, 0, 'h'},
        {0, 0, 0, 0},
    };

    int opt, option_index = 0;
    while ((opt = getopt_long(argc, (char *const *)argv, "k:i:o:h::?::", long_options, &option_index)) != -1) {
        switch (opt) {
        case KEY_OPT:
            args.key = optarg;
            break;
        case IMG_OPT:
            args.image = optarg;
            break;
        case OUT_OPT:
            args.out = optarg;
            break;
        case '?':
            help();
            return EXIT_SUCCESS;
        case 'h':
            help();
            return EXIT_SUCCESS;
        default:
            return EXIT_FAILURE;
        }
    }

    /* Argument parsing */
    args.command = get_command(argv[optind]);
    args.files_amount = argc - optind - 1;
    args.files = argv + optind + 1;

    if (!validate_args(args)) {
        return EXIT_FAILURE;
    }
    
    /* Setup logging */
    const char *LOGFILE = "log.log";
    log_file = fopen(LOGFILE, "a");
    if (!log_file) {
        fprintf(stderr, "Failed to create %s", LOGFILE);
        cleanup();
        return EXIT_FAILURE;
    }

    cleanup();
    return EXIT_SUCCESS;
}

void cleanup() {
    free_page();
    close_log();
}

void segv_handler(int sig) {
    fprintf(stderr, "%d: Read of protected memory detected\n", sig);
    log_custom_message("\tExited programm with segmentation fault\n\n");
    cleanup();
    _exit(sig);
}

void sigint_handler(int sig) {
    fprintf(stderr, "%d: Keyboard interruption\n", sig);
    log_custom_message("\tExited program with keyboard interruption\n\n");
    cleanup();
    _exit(sig);
}
