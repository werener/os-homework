#include "image.h"
#include "logging.h"

#include <asm-generic/errno.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

int count_files(const char *path) {
    FILE *img_f = fopen(path, "rb");
    if (!img_f) {
        fprintf(stderr, "Image file '%s' couldn't be opened\n", path);
        return IMAGE_ERROR;
    }

    fseek(img_f, 0, SEEK_END);
    long size = ftell(img_f);
    rewind(img_f);

    int filecount = 0;
    metadata_t metadata;
    while (ftell(img_f) != size) {
        // Not enough metadata, though there should be
        if ((long)sizeof(metadata_t) + ftell(img_f) > size) {
            fclose(img_f);
            fprintf(stderr, "Wrong file format: '%s'\n", path);
            return IMAGE_ERROR;
        }

        fread(&metadata, sizeof(metadata_t), 1, img_f);
        int bytes_till_next_file = metadata.data_len + metadata.name_len;
        // Metadata is incorrect
        if (ftell(img_f) + bytes_till_next_file > size) {
            fclose(img_f);
            fprintf(stderr, "Wrong file format: '%s'\n", path);
            return IMAGE_ERROR;
        }

        filecount++;
        fseek(img_f, bytes_till_next_file, SEEK_CUR);
    }
    fclose(img_f);
    return filecount;
}

byte *add_salt(char salt[SALT_SIZE], byte *key, size_t len_key) {
    byte *full_key = malloc(SALT_SIZE + len_key + 1);
    memcpy(full_key, salt, SALT_SIZE);
    memcpy(full_key + SALT_SIZE, key, len_key + 1);

    return full_key;
}

pthread_mutex_t fetch_file_mx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t image_mx = PTHREAD_MUTEX_INITIALIZER;

void *add_worker(void *arg) {
    add_args_t *args = (add_args_t *)arg;
    while (1) {
        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += DEADLOCK_DETECTION_TIMER_SEC;

        int lock_res = pthread_mutex_timedlock(&fetch_file_mx, &timeout);
        if (lock_res == ETIMEDOUT) {
            fprintf(stderr, "Possible deadlock\n");
            write_to_log("-", "ERROR - Deadlock");
            break;
        }

        // fetch file if any left
        if (args->processed >= args->files->count) {
            pthread_mutex_unlock(&fetch_file_mx);
            break;
        }

        int currently_processing = args->processed;
        args->processed++;
        pthread_mutex_unlock(&fetch_file_mx);

        char *cur_name = args->files->data[currently_processing];
        fprintf(stderr, "(%i/%li) Processing '%s'\n", currently_processing, args->files->count, cur_name);
		

        // start processing file
        FILE *cur_f = fopen(cur_name, "rb");
        if (!cur_f) {
            fprintf(stderr, "Couldn't open %s\n", cur_name);
            continue;
        }

        metadata_t metadata;
        // get data len
        fseek(cur_f, 0, SEEK_END);
        metadata.data_len = ftell(cur_f);
        rewind(cur_f);

        // get name len
        metadata.name_len = strlen(cur_name);

        // get random salt
        FILE *urandom = fopen("/dev/urandom", "rb");
        fread(metadata.salt, 1, SALT_SIZE, urandom);
        fclose(urandom);

        // prepare to write into image
        byte *full_key = add_salt(metadata.salt, args->key, args->len_key);
        state_t *state = rc4_init(full_key);
        size_t bytes_remained = metadata.data_len;
        byte *buffer = malloc(BUFFER_SIZE);

		write_to_log(cur_name, "In process");
        // start writing current file into image
        pthread_mutex_lock(&image_mx);

		fwrite(&metadata, 1, sizeof(metadata_t), args->img_f);
		fwrite(cur_name, 1, metadata.name_len, args->img_f);
        while (bytes_remained > 0) {
            size_t chunk_size = bytes_remained >= BUFFER_SIZE
                                    ? BUFFER_SIZE
                                    : bytes_remained;

            bytes_remained -= fread(buffer, 1, chunk_size, cur_f);

            rc4_apply(state, buffer, chunk_size);
            fwrite(buffer, 1, chunk_size, args->img_f);
        }

        pthread_mutex_unlock(&image_mx);
		write_to_log(cur_name, "Finished");
		
        fclose(cur_f);
        free_state(state);
        free(full_key);
        free(buffer);
    }

    return NULL;
}