#include "image.h"

int count_files(const char *path) {
	FILE *img_f = fopen(path, "rb");
	if (!img_f) {
		fprintf(stderr, "Image file %s couldn't be opened\n", path);
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
			fprintf(stderr, "Wrong file format: %s\n", path);
			return IMAGE_ERROR;
		}

		fread(&metadata, sizeof(metadata_t), 1, img_f);
		int bytes_till_next_file = metadata.data_len + metadata.name_len;
		// Metadata is incorrect
		if (ftell(img_f) + bytes_till_next_file > size) {
			fclose(img_f);
            fprintf(stderr, "Wrong file format: %s\n", path);
			return IMAGE_ERROR;
		}

		filecount++;
		fseek(img_f, bytes_till_next_file, SEEK_CUR);
	}
	fclose(img_f);
	return filecount;
}
