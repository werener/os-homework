#pragma once

typedef struct {
    char *bin_title;
    char *key;
    char *image;
    char *out;
    char *command;

    int files_amount;
    char **files;
} cli_args_t;

void help();

int validate_args(cli_args_t);