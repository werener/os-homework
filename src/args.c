#include "args.h"

#include <stdio.h>

int validate_args(cli_args_t args) {
    if (args.command == NULL) {
        fprintf(stderr, "No provided subcommand\n");
        help();
        return 1;
    }

    return 0;
};

void help() {
    printf(
        "List of availiable commands\n"

        "\nadd\tEncrypts [FILES] using the provided [KEY] and writes them to [IMAGE]\n"
        "\tUsage: add --key=[KEY] --image=[IMAGE] [FILES...]\n"
        "\tRequires: key, path to image and at least one file\n"

        "\nlist\tLists all files, contained in the provided [IMAGE]\n"
        "\tUsage: list --image=[IMAGE]\n"
        "\tRequires: path to image\n"

        "\nget\tDecrypts [FILE], using the provided [KEY] and writes it to [OUT]\n"
        "\tUsage: get --key=[KEY] --image=[IMAGE] --out=[OUT] [FILE]\n"
        "\tRequires: key, path to image, path to the output file and exactly one file\n");
}
