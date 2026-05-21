#pragma once
#include <stdbool.h>

typedef enum {
    CMD_ADD,
    CMD_LIST,
    CMD_GET,
    CMD_NONE,
    CMD_UNKNOWN
} Command;

Command get_command(const char*);


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

bool validate_args(const cli_args_t);