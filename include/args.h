#pragma once

#include "image_operations.h"
#include "logging.h"

#include <stdbool.h>

typedef enum {
    CMD_ADD,
    CMD_LIST,
    CMD_GET,
    CMD_NONE,
    CMD_UNKNOWN
} Command;

Command get_command(const char *);

typedef struct {
    char *bin_title;
    char *key;
    char *image;
    char *out;
    Command command;

    int files_amount;
    char **files;
} cli_args_t;

void help();

bool validate_args(const cli_args_t args);
void parse_arguments(int argc, char **argv, cli_args_t *args);