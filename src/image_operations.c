#include "image_operations.h"

#include <stdlib.h>
#include <string.h>
#include <strings.h>

int compare_names(const void *ln, const void *rn) {
	const char *ls = (const char *)ln;
	const char *rs = (const char *)rn;

	return strcasecmp(rs, ls);
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
	char **filenames = malloc(file_count * sizeof(char *));
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
	}
}

void get(const char *img_path, byte *key, const char *file, const char *out) {
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
	metadata_t metadata;
	for (int i = 0; i < file_count; ++i) {
		// read metadata of the file
		fread(&metadata, sizeof(metadata_t), 1, img_f);
		// check if the name matches
		char *name_temp = malloc(metadata.name_len + 1);
		fread(name_temp, metadata.name_len, 1, img_f);

		printf("%s\n", name_temp);
		printf("salt: %4s\n", metadata.salt);
		printf("dl: %d, nl: %d\n", metadata.data_len, metadata.name_len);

		if (strcasecmp(name_temp, file) == 0) {
			FILE *result_f = fopen(out, "wb");
			if (!result_f) {
				fprintf(stderr, "Cannot open %s", out);
				free(name_temp);
				return;
			}

			int key_len = strlen((char *)key);
			byte *full_key = malloc(SALT_SIZE + key_len + 1);
			memcpy(full_key, metadata.salt, SALT_SIZE);
			memcpy(full_key + SALT_SIZE, key, key_len + 1);

			state_t *state = rc4_init(full_key);

			size_t bytes_remained = metadata.data_len;
			byte *buffer = malloc(BUFFER_SIZE);
			while (bytes_remained > 0) {
				size_t chunk_size = bytes_remained >= BUFFER_SIZE
										? BUFFER_SIZE
										: bytes_remained;

				bytes_remained -= fread(buffer, 1, chunk_size, img_f);

				rc4_apply(state, buffer, chunk_size);
				fwrite(buffer, 1, chunk_size, result_f);
			}
			free(buffer);
			free(name_temp);
			free_state(state);
			fclose(result_f);
			return;
		}
		// free(name_temp);
		// go to the next metadata block
		fseek(img_f, metadata.data_len, SEEK_CUR);
	}
	fclose(img_f);
}