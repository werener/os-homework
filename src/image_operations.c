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

	// note that count_files also includes image validation
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

// void get(const char *img_path, char *key, const char *src_file,
// 		 const char *result_path) {
// 	FILE *img_f = fopen(img_path, "rb");
// 	if (!img_f) {
// 		fprintf(stderr, "Image file %s doesn't exist", img_path);
// 		return;
// 	}

// 	FILE *result_f = fopen(result_path, "wb");
// 	if (!result_f) {
// 		fprintf(stderr, "Cannot open %s", result_path);
// 		fclose(img_f);
// 		return;
// 	}

// 	// TODO: DECRYPT file->data

// 	// if (fwrite(file->data, file->data_len, 1, result_f) !=
// 	// 	(size_t)file->data_len) {
// 	// 	fprintf(stderr, "Write failed!");
// 	// }
// 	fclose(img_f);
// 	fclose(result_f);

// 	(void)key;
// }