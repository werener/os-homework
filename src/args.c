#include "args.h"

#include <stdio.h>
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
    printf(
        "\nadd\tEncrypts [FILES] using the provided [KEY] and writes them to [IMAGE]\n"
        "\tUsage: add --key=[KEY] --image=[IMAGE] [FILES...]\n"
        "\tRequires: key, path to image and at least one file\n");
}
void help_list() {
    printf(
        "\nlist\tLists all files, contained in the provided [IMAGE]\n"
        "\tUsage: list --image=[IMAGE]\n"
        "\tRequires: path to image\n");
}
void help_get() {
    printf(
        "\nget\tDecrypts [FILE], using the provided [KEY] and writes it to [OUT]\n"
        "\tUsage: get --key=[KEY] --image=[IMAGE] --out=[OUT] [FILE]\n"
        "\tRequires: key, path to image, path to the output file and exactly one file\n");
}
void help() {
    printf("HELP MENU\n");
    help_add();
    help_list();
    help_get();
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

    // Requires: key, path to image, path to the output file and exactly one file
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
            fprintf(stderr, "More than one file provided\n");
            help_get();
            return false;
        }
        return true;

    default:
        perror("Something went incredibly wrong");
        return false;
    }
}
