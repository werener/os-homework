#include "image.h"
#include "logging.h"

#include <asm-generic/errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t fetch_file_mx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t image_mx = PTHREAD_MUTEX_INITIALIZER;

int count_files(const char *path) {
	FILE *img_f = fopen(path, "rb");
	if (!img_f) {
		fprintf(stderr, "Image file '%s' couldn't be opened\n", path);
		return IMAGE_ERROR;
	}

	fseek(img_f, 0, SEEK_END);
	uint64_t size = ftell(img_f);
	rewind(img_f);

	int filecount = 0;
	metadata_t metadata;
	while ((uint64_t)ftell(img_f) != size) {

		// Not enough metadata, though there should be
		if ((size_t)sizeof(metadata_t) + ftell(img_f) > size) {
			fclose(img_f);
			fprintf(stderr, "Wrong file format: '%s'\n", path);
			return IMAGE_ERROR;
		}
		fread(&metadata, 1, sizeof(metadata_t), img_f);
		uint64_t bytes_till_next_file =
			(uint64_t)metadata.data_len + (uint64_t)metadata.name_len;
		// Metadata is incorrect
		if ((size_t)ftell(img_f) + bytes_till_next_file > size) {
			fclose(img_f);
			fprintf(stderr, "Wrong file format: '%s'\n", path);
			return IMAGE_ERROR;
		}
		fseek(img_f, bytes_till_next_file, SEEK_CUR);
		filecount++;
	}
	fclose(img_f);
	return filecount;
}

byte *add_salt(byte salt[SALT_SIZE], byte *key, size_t len_key) {
	byte *full_key = malloc(len_key + SALT_SIZE);
	memcpy(full_key, key, len_key);
	memcpy(full_key + len_key, salt, SALT_SIZE);

	return full_key;
}

void *add_worker(void *arg) {
	add_args_t *args = (add_args_t *)arg;
	struct timespec timeout;

	while (1) {
		clock_gettime(CLOCK_REALTIME, &timeout);
		timeout.tv_sec += DEADLOCK_DETECTION_TIMER_SEC;

		int s = pthread_mutex_timedlock(&fetch_file_mx, &timeout);
		if (s == ETIMEDOUT) {
			fprintf(
				stderr,
				"Deadlock detected on fetching file task. Exiting thread\n");
			return NULL;
		}

		if (args->processed >= args->files->count) {
			pthread_mutex_unlock(&fetch_file_mx);
			break;
		}

		size_t currently_processing = args->processed;
		args->processed++;
		pthread_mutex_unlock(&fetch_file_mx);

		char *cur_name = args->files->data[currently_processing];
		FILE *cur_f = fopen(cur_name, "rb");
		if (!cur_f) {
			fprintf(stderr, "File '%s' couldn't be opened. Skipping\n",
					cur_name);
			write_to_log(cur_name, "Error opening file");
			continue;
		}

		metadata_t metadata;
		fseek(cur_f, 0, SEEK_END);
		metadata.data_len = ftell(cur_f);
		rewind(cur_f);

		metadata.name_len = strlen(cur_name);

		for (int i = 0; i < SALT_SIZE; ++i) {
			metadata.salt[i] = (byte)(rand() & 0xFF);
		}

		write_to_log(cur_name, "In process");

		byte *full_key = add_salt(metadata.salt, args->key, args->len_key);
		state_t *state = rc4_init(full_key, args->len_key + SALT_SIZE);

		size_t img_filesize =
			sizeof(metadata_t) + metadata.name_len + metadata.data_len;

        // start fetching offset for cur_file
		pthread_mutex_lock(&image_mx);

		off_t map_offset = (args->curr_offset / PAGE_SIZE) * PAGE_SIZE;
		size_t delta = args->curr_offset - map_offset;
		size_t map_len = delta + img_filesize;

		// extend/truncate image file to have its memory aligned
		if (ftruncate(args->img_fd, map_offset + map_len) == -1) {
			fprintf(stderr, "Failed truncation on offset %ld for size %ld\n", map_offset, map_len);
			pthread_mutex_unlock(&image_mx);
			free(full_key);
			free_state(state);
			fclose(cur_f);
			continue;
		}

		byte *img_map = mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED,
							 args->img_fd, map_offset);

        // 
		args->curr_offset += img_filesize;
		
        pthread_mutex_unlock(&image_mx);

        // continue the actual writing to the fetched offset
		if (img_map == MAP_FAILED) {
			fprintf(stderr, "mmap failed for file '%s'\n", cur_name);
			free(full_key);
			free_state(state);
			fclose(cur_f);
			continue;
		}

		byte *img = img_map + delta;

		// write metadata and name
		memcpy(img, &metadata, sizeof(metadata_t));
		img += sizeof(metadata_t);
		memcpy(img, cur_name, metadata.name_len);
		img += metadata.name_len;

		// start encryption
		size_t bytes_remained = metadata.data_len;
		byte *buffer = malloc(BUFFER_SIZE);
		while (bytes_remained > 0) {
			size_t chunk_size =
				bytes_remained >= BUFFER_SIZE ? BUFFER_SIZE : bytes_remained;
			size_t read_bytes = fread(buffer, 1, chunk_size, cur_f);
			if (read_bytes == 0)
				break;

			rc4_apply(state, buffer, read_bytes);
			memcpy(img, buffer, read_bytes);
			img += read_bytes;
			bytes_remained -= read_bytes;
		}

        // Deallocate any mapping for the region starting at ADDR and extending LEN
		munmap(img_map, map_len);
		
        free(buffer);
		free(full_key);
		free_state(state);
		fclose(cur_f);

		write_to_log(cur_name, "Finished");
	}
	return NULL;
}