#include "caesar.h"
#include "logging.h"
#include "secure_copy.h"
#include "string.h"

#include <getopt.h>
#include <signal.h>
#include <unistd.h>

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
	int opt, option_index = 0;
	int mode = MODE_AUTO;
	static struct option long_options[] = {{"mode", optional_argument, 0, 'm'}, {0, 0, 0, 0}};

	while ((opt = getopt_long(argc, (char *const *)argv, "m:", long_options, &option_index)) !=
		   -1) {
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
	if (mode == MODE_AUTO)
		mode = MODE_PARALLEL; // TODO: add some heuristic to identify the need for parallelism

	log_custom_message("\tStarted execution\n");

	pthread_t pool[WORKER_COUNT];

	switch (mode) {
	case MODE_PARALLEL:
		for (int i = 0; i < WORKER_COUNT; ++i)
			pthread_create(&pool[i], NULL, worker, &args);
		for (int i = 0; i < WORKER_COUNT; ++i)
			pthread_join(pool[i], NULL);

		break;

	case MODE_SEQUENTIAL:
		pthread_create(&pool[0], NULL, worker, &args);
		pthread_join(pool[0], NULL);
	}

CLEANUP:
	log_custom_message("\tSuccessfully finished execution\n\n");
	free_page();
	close_log();
	return EXIT_CODE;
}

void segv_handler(int sig) {
	INTERRUPTION = sig;
	fprintf(stderr, "%d: Read of protected memory detected\n", sig);
	log_custom_message("\tExited programm with segmentation fault\n\n");
	free_page();
	close_log();
	_exit(sig);
}

void sigint_handler(int sig) {
	INTERRUPTION = sig;
	fprintf(stderr, "%d: Keyboard interruption\n", sig);
	log_custom_message("\tExited program with keyboard interruption\n\n");
	close_log();
	free_page();
	_exit(sig);
}
