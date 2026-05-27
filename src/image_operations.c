#include "image_operations.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define WORKER_COUNT 4

int compare_names(const void *ln, const void *rn) {
    const char *const *ls = (const char *const *)ln;
    const char *const *rs = (const char *const *)rn;

    return strcasecmp(*ls, *rs);
}

void list(const char *img_path) {
    // also includes validation
    int file_count = count_files(img_path);
    if (file_count == IMAGE_ERROR) {
        return;
    }
    if (file_count == 0) {
        fprintf(stderr, "Empty image\n");
        return;
    }

    FILE *img_f = fopen(img_path, "rb");
    char *filenames[file_count];
    metadata_t metadata;
    for (int i = 0; i < file_count; ++i) {
        // read metadata of the file
        fread(&metadata, sizeof(metadata_t), 1, img_f);
        // save its name
        filenames[i] = malloc(metadata.name_len + 1);
        fread(filenames[i], metadata.name_len, 1, img_f);
        // go to the next metadata block
        fseek(img_f, metadata.data_len, SEEK_CUR);
    }

    qsort(filenames, file_count, sizeof(sizeof(char *)), compare_names);

    for (int i = 0; i < file_count; ++i) {
        if (strcmp(filenames[i], "\0"))
            printf("- %s\n", filenames[i]);
        free(filenames[i]);
    }
    fclose(img_f);
}

void get(const char *img_path, byte *key, const char *name_searched, const char *out) {
    // also includes validation
    int file_count = count_files(img_path);
    if (file_count == IMAGE_ERROR) {
        return;
    }
    if (file_count == 0) {
        printf("Empty image\n");
        return;
    }

    FILE *img_f = fopen(img_path, "rb");
    metadata_t metadata;
    for (int i = 0; i < file_count; ++i) {
        // read metadata of the file
        fread(&metadata, sizeof(metadata_t), 1, img_f);

        // check if the name matches
        char *name_cur = malloc(metadata.name_len + 1);
        fread(name_cur, metadata.name_len, 1, img_f);
        if (strcasecmp(name_cur, name_searched) == 0) {

            // try opening [out]
            FILE *out_f = fopen(out, "wb");
            if (!out_f) {
                fprintf(stderr, "Cannot open %s", out);
                free(name_cur);
                return;
            }

            // concatenate salt and the provided [key]
            byte *full_key = add_salt(metadata.salt, key, strlen((char *)key));

            state_t *state = rc4_init(full_key);
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
            return;
        }

        // go to the next metadata block
        fseek(img_f, metadata.data_len, SEEK_CUR);
        free(name_cur);
    }
    fclose(img_f);
}

void add(const char *img_path, byte *key, char **files, int file_amount) {
    FILE *img_f = fopen(img_path, "a");
    if (!img_f) {
        fprintf(stderr, "Image file %s doesn't exist. Creating it\n", img_path);
        img_f = fopen(img_path, "w");
        if (!img_f) {
            fprintf(stderr, "Cannot create %s\n", img_path);
            return;
        }
    }
	fclose(img_f);
    img_f = fopen(img_path, "a");

    FILE *urandom = fopen("/dev/urandom", "rb");
    if (!urandom) {
        fprintf(stderr, "Cannot open '/dev/urandom'\n");
        return;
    }
    fclose(urandom);

    add_args_t args = {
        .files = unwind_folders(files, file_amount),
        .processed = 0,
        .key = key,
        .len_key = strlen((char *)key),
        .img_f = img_f,
    };

    pthread_t pool[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; ++i) {
        pthread_create(&pool[i], NULL, add_worker, &args);
    }
    for (int i = 0; i < WORKER_COUNT; ++i) {
        pthread_join(pool[i], NULL);
    }
    fclose(img_f);
    array_free(args.files);
}