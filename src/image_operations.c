#include "image_operations.h"
#include "logging.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define MAX_ALLOWED_FILENAME ((uint32_t)63)
#define WORKER_COUNT 4

typedef struct {
    char *name;
    uint32_t data_len;
} file_entry_t;

int compare_entries(const void *a, const void *b) {
    const file_entry_t *fa = (const file_entry_t *)a;
    const file_entry_t *fb = (const file_entry_t *)b;
    return strcmp(fa->name, fb->name);
}

int list(const char *img_path) {
    fprintf(log_file, "\nListing files from '%s'\n", img_path);

    // also includes validation
    int file_count = count_files(img_path);
    if (file_count == IMAGE_ERROR) {
        return EXIT_FAILURE;
    }
    if (file_count == 0) {
        fprintf(stderr, "Empty image\n");
        return EXIT_SUCCESS;
    }

    FILE *img_f = fopen(img_path, "rb");
    file_entry_t entries[file_count];
    metadata_t metadata;
    for (int i = 0; i < file_count; ++i) {
        // read metadata of the file
        fread(&metadata, sizeof(metadata_t), 1, img_f);
        // save its name
        entries[i].data_len = metadata.data_len;
        // if the name is too large, show only first 60 bytes and show '...'
        uint32_t allowed_size = metadata.name_len > MAX_ALLOWED_FILENAME
                                   ? MAX_ALLOWED_FILENAME
                                   : metadata.name_len;
        entries[i].name = malloc(allowed_size + 1);
        fread(entries[i].name, allowed_size, 1, img_f);
        if (allowed_size == MAX_ALLOWED_FILENAME) {
            for (int j = 0; j < 3; ++j)
                entries[i].name[allowed_size - j - 1] = '.';
        }
        fseek(img_f, metadata.name_len - allowed_size, SEEK_CUR);
        
        // go to the next metadata block
        fseek(img_f, metadata.data_len, SEEK_CUR);
    }

    qsort(entries, file_count, sizeof(file_entry_t), compare_entries);

    printf("Image contains %d files\n", file_count);
    for (int i = 0; i < file_count; ++i) {
        file_entry_t entry = entries[i];
        printf("- '%s' (%u Bytes)\n", entry.name, entry.data_len);
        free(entry.name);
    }
    fclose(img_f);

    return EXIT_SUCCESS;
}

int get(const char *img_path, byte *key, const char *name_searched, const char *out) {
    fprintf(log_file, "\nSaving '%s' from '%s' to '%s'\n", name_searched, img_path, out);

    // also includes validation
    int file_count = count_files(img_path);
    if (file_count == IMAGE_ERROR) {
        return EXIT_FAILURE;
    }
    if (file_count == 0) {
        printf("Empty image\n");
        return EXIT_SUCCESS;
    }

    FILE *img_f = fopen(img_path, "rb");
    metadata_t metadata;
    for (int i = 0; i < file_count; ++i) {
        // read metadata of the file
        fread(&metadata, sizeof(metadata_t), 1, img_f);

        
        // if the name is too large, show only first 60 bytes and show '...'
        int32_t allowed_size = metadata.name_len > MAX_ALLOWED_FILENAME
                                   ? MAX_ALLOWED_FILENAME
                                   : metadata.name_len;

        char *name_cur = malloc(allowed_size + 1);
        
        fread(name_cur, allowed_size, 1, img_f);
        fseek(img_f, metadata.name_len - allowed_size, SEEK_CUR);
        // check if the name matches
        if (strcasecmp(name_cur, name_searched) == 0) {

            // try opening [out]
            FILE *out_f = fopen(out, "wb");
            if (!out_f) {
                fprintf(stderr, "Cannot open '%s'", out);
                free(name_cur);
                return EXIT_FAILURE;
            }

            // concatenate salt and the provided [key]
            byte *full_key = add_salt(metadata.salt, key, strlen((char *)key));

            state_t *state = rc4_init(full_key, strlen((char *)key) + SALT_SIZE);
            size_t bytes_remained = metadata.data_len;
            byte *buffer = malloc(BUFFER_SIZE);
            while (bytes_remained > 0) {
                size_t chunk_size = bytes_remained >= BUFFER_SIZE
                                        ? BUFFER_SIZE
                                        : bytes_remained;

                bytes_remained -= fread(buffer, 1, chunk_size, img_f);

                rc4_apply(state, buffer, chunk_size);
                fwrite(buffer, 1, chunk_size, out_f);
            }
            free(buffer);
            free(full_key);
            free(name_cur);
            free_state(state);
            fclose(out_f);
            fclose(img_f);
            return EXIT_SUCCESS;
        }

        // go to the next metadata block
        fseek(img_f, metadata.data_len, SEEK_CUR);
        free(name_cur);
    }
    fclose(img_f);
    printf("No file '%s' on image '%s'\n", name_searched, img_path);
    return EXIT_SUCCESS;
}

int add(const char *img_path, byte *key, char **files, int file_amount) {
    add_args_t args = {
        .files = unwind_folders(files, file_amount),
        .processed = 0,
        .key = key,
        .len_key = strlen((char *)key),
        .img_f = NULL,
    };
    fprintf(log_file, "\nAdding files (%ld) to '%s'\n", args.files->count, img_path);

    FILE *img_f = fopen(img_path, "a");
    if (!img_f) {
        fprintf(stderr, "Image file '%s' doesn't exist. Creating it\n", img_path);
        img_f = fopen(img_path, "w");
        if (!img_f) {
            fprintf(stderr, "Cannot create '%s'\n", img_path);
            return EXIT_FAILURE;
        }
    }
    fclose(img_f);
    img_f = fopen(img_path, "a");
    args.img_f = img_f;

    srand(time(NULL));
    pthread_t pool[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; ++i) {
        pthread_create(&pool[i], NULL, add_worker, &args);
    }
    for (int i = 0; i < WORKER_COUNT; ++i) {
        pthread_join(pool[i], NULL);
    }
    fclose(img_f);
    array_free(args.files);

    return EXIT_SUCCESS;
}