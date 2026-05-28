#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Command get_command(const char *repr) {
	if (repr == NULL)
		return CMD_NONE;
	if (strcmp(repr, "add") == 0)
		return CMD_ADD;
	if (strcmp(repr, "list") == 0)
		return CMD_LIST;
	if (strcmp(repr, "get") == 0)
		return CMD_GET;
	return CMD_UNKNOWN;
}

void help_add() {
	printf("\nadd\tEncrypts [FILES] using the provided [KEY] and writes them "
		   "to [IMAGE]\n"
		   "\tUsage: add --key=[KEY] --image=[IMAGE] [FILES...]\n"
		   "\tRequires: key, path to image and at least one file\n");
}
void help_list() {
	printf("\nlist\tLists all files, contained in the provided [IMAGE]\n"
		   "\tUsage: list --image=[IMAGE]\n"
		   "\tRequires: path to image\n");
}
void help_get() {
	printf("\nget\tDecrypts [FILE] from the [IMAGE], using the provided [KEY] "
		   "and writes it to "
		   "[OUT]\n"
		   "\tUsage: get --key=[KEY] --image=[IMAGE] --out=[OUT] [FILE]\n"
		   "\tRequires: key, path to image, path to the output file and "
		   "exactly one file\n");
}
void help() {
	printf("HELP MENU\n");
	help_add();
	help_list();
	help_get();
}

void parse_arguments(int argc, char **argv, cli_args_t *args) {
	args->bin_title = argv[0];
	args->key = NULL;
	args->image = NULL;
	args->out = NULL;
	args->command = CMD_NONE;
	args->files = NULL;
	args->files_amount = 0;

	// First pass: identify command and options
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-add") == 0) {
			args->command = CMD_ADD;
		} else if (strcmp(argv[i], "-list") == 0) {
			args->command = CMD_LIST;
		} else if (strcmp(argv[i], "-get") == 0) {
			args->command = CMD_GET;
		}
	}

	// Second pass: parse options based on command
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-key") == 0 && i + 1 < argc) {
			args->key = argv[++i];
		} else if (strcmp(argv[i], "-image") == 0 && i + 1 < argc) {
			args->image = argv[++i];
		} else if (strcmp(argv[i], "-out") == 0 && i + 1 < argc) {
			args->out = argv[++i];
		}
	}

	// Third pass: collect non-option arguments (files/directories)
	if (args->command == CMD_ADD) {
		args->files = malloc((argc + 1) * sizeof(char *));
		args->files_amount = 0;

		for (int i = 1; i < argc; i++) {
			if (argv[i][0] == '-') {
				if (strcmp(argv[i], "-key") == 0 ||
					strcmp(argv[i], "-image") == 0) {
				}
				continue;
			}

			if (i > 1 && (strcmp(argv[i - 1], "-key") == 0 ||
						  strcmp(argv[i - 1], "-image") == 0)) {
				continue;
			}

			args->files[args->files_amount++] = argv[i];
		}
	} else if (args->command == CMD_GET) {
		// For GET: the last argument is file_name
		if (argc > 1 && args->out != NULL) {
			// Find the last non-option argument
			for (int i = argc - 1; i >= 1; i--) {
				if (argv[i][0] != '-') {
					// Check if it's not a value for -key, -image, or -out
					if (i > 1 && (strcmp(argv[i - 1], "-key") == 0 ||
								  strcmp(argv[i - 1], "-image") == 0 ||
								  strcmp(argv[i - 1], "-out") == 0)) {
						continue;
					}
					args->files = malloc(sizeof(char *));
					args->files[0] = argv[i];
					args->files_amount = 1;
					break;
				}
			}
		}
	}
}

bool validate_args(const cli_args_t args) {
	switch (args.command) {
	case CMD_NONE:
		fprintf(stderr, "Command not provided\n");
		help();
		return false;
	case CMD_UNKNOWN:
		fprintf(stderr, "Unknown command\n");
		help();
		return false;

	// Requires: key, path to image and at least one file
	case CMD_ADD:
		if (args.key == NULL) {
			fprintf(stderr, "Key not provided\n");
			help_add();
			return false;
		}
		if (args.image == NULL) {
			fprintf(stderr, "Image not provided\n");
			help_add();
			return false;
		}
		if (args.files_amount < 1) {
			fprintf(stderr, "Files not provided provided\n");
			help_add();
			return false;
		}
		return true;

	// Requires: path to image
	case CMD_LIST:
		if (args.image == NULL) {
			fprintf(stderr, "Image not provided\n");
			help_list();
			return false;
		}
		return true;

	// Requires: key, path to image, path to the output file and exactly one
	// file
	case CMD_GET:
		if (args.key == NULL) {
			fprintf(stderr, "Key not provided\n");
			help_get();
			return false;
		}
		if (args.image == NULL) {
			fprintf(stderr, "Image not provided\n");
			help_get();
			return false;
		}
		if (args.out == NULL) {
			fprintf(stderr, "Path to the output file not provided\n");
			help_get();
			return false;
		}
		if (args.files_amount != 1) {
			fprintf(stderr, "Not exactly one file provided\n");
			help_get();
			return false;
		}
		return true;

	default:
		perror("Something went incredibly wrong");
		return false;
	}
}
