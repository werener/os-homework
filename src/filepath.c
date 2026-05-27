#include "filepath.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

char *normalize_path(char *path);
void collect_file(array_t *arr, char *path);

array_t *unwind_folders(char **files, int files_amount) {
    array_t *arr = array_init();
    for (int i = 0; i < files_amount; ++i) {
        collect_file(arr, files[i]);
    }
    return arr;
}

void collect_file(array_t *arr, char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        fprintf(stderr, "Cannot get status for '%s'\n", path);
        return;
    }

    // easy-handled cases
    if (S_ISREG(st.st_mode)) {
        array_push(arr, normalize_path(path));
		return;
    }
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "'%s' is neither a folder, nor a regular file. Cannot process it\n", path);
        return;
    }

    // folder recursive traversal
    DIR *dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "Failed to open folder '%s'\n", path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // skip . and .. entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // "path" + '/' + "entry" + '\0'
        size_t full_len = strlen(path) + 1 + strlen(entry->d_name) + 1;
        char *full_path = malloc(full_len);
        snprintf(full_path, full_len, "%s/%s", path, entry->d_name);

        // recursively descent down into folder structure
        collect_file(arr, normalize_path(full_path));
    }
    closedir(dir);
}

char *normalize_path(char *path) {
    size_t len = strlen(path);
    if (len == 0) {
        return strdup("");
    }

    char *result = malloc(len + 1);

    size_t j = 0;
    int prev_was_slash = 0;

    // add slash only if it isn't a duplicate
    for (size_t i = 0; i < len; i++) {
        if (path[i] == '/') {
            if (!prev_was_slash) {
                result[j++] = '/';
            }
            prev_was_slash = 1;
        } else {
            result[j++] = path[i];
            prev_was_slash = 0;
        }
    }

    // revert back the leading '/'
    if (j > 0 && result[j - 1] == '/') {
        j--;
    }

    result[j] = '\0';
    return result;
}