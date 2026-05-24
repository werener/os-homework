#include "image.h"

#include <stdlib.h>
#include <strings.h>

const long METADATA_SIZE = sizeof(int32_t) + sizeof(int32_t) + SALT_SIZE;

void print_file(file_t file) {
    printf("%ld\n", METADATA_SIZE);
    printf("Salt (%d): ", SALT_SIZE);
    for (int i = 0; i < SALT_SIZE; ++i) {
        printf("%02x ", file.salt[i]);
    }
    printf("\nName (%d): %s\n", file.name_len, file.name);
    printf("Data (%d): %s\n\n", file.data_len, file.data);
}

int count_files(char *path) {
    FILE *img_f = fopen(path, "rb");
    if (!img_f) {
        fprintf(stderr, "Image file %s couldn't be opened\n", path);
        return IMAGE_ERROR;
    }

    fseek(img_f, 0, SEEK_END);
    long size = ftell(img_f);
    rewind(img_f);

    int filecount = 0;
    file_t f;
    while (1) {
        // Successfully read until the very end
        if (ftell(img_f) == size) {
            break;
        }
        // Not enough metadata, though there should be
        if (METADATA_SIZE + ftell(img_f) > size) {
            fclose(img_f);
            return IMAGE_ERROR;
        }

        fread(&f, METADATA_SIZE, 1, img_f);
        int bytes_till_next_file = f.data_len + f.name_len;

        // Metadata is incorrect
        if (ftell(img_f) + bytes_till_next_file > size) {
            fclose(img_f);
            return IMAGE_ERROR;
        }

        filecount++;
        fseek(img_f, bytes_till_next_file, SEEK_CUR);
    }
    fclose(img_f);
    return filecount;
}

image_t *get_image(char *path) {
    FILE *img_f = fopen(path, "rb");
    if (!img_f) {
        printf("Cannot open file '%s' for read", path);
        return NULL;
    }

    int amount_of_files = count_files(path);
    if (amount_of_files == IMAGE_ERROR) {
        fprintf(stderr, "Image file %s is of wrong format or corrupted\n", path);
        fclose(img_f);
        return NULL;
    }

    // Main logic
    int i = 0;
    file_t *files = malloc(sizeof(file_t) * amount_of_files);
    while (i < amount_of_files) {
        file_t *file = &files[i];
        fread(file, METADATA_SIZE, 1, img_f);

        file->name = malloc(file->name_len);
        file->data = malloc(file->data_len);
        fread(file->name, file->name_len, 1, img_f);
        fread(file->data, file->data_len, 1, img_f);

        i++;
    }
    image_t *final_image = malloc(sizeof(image_t));
    final_image->files = files;
    final_image->files_amount = amount_of_files;

    fclose(img_f);
    return final_image;
}

int compare_names(const void *ln, const void *rn) {
    const file_t *lf = (const file_t *)ln;
    const file_t *rf = (const file_t *)rn;

    if (lf->name == NULL && rf->name == NULL)
        return 0;
    if (lf->name == NULL)
        return -1;
    if (rf->name == NULL)
        return 1;

    return strcasecmp(lf->name, rf->name);
}

void sort_image(image_t *image) {
    if (image->files == NULL) {
        return;
    }
    qsort(
        image->files,
        image->files_amount,
        sizeof(file_t),
        compare_names);
}

file_t *get_by_name(image_t *image, char *name) {
    int name_len = strlen(name);
    for (int i = 0; i < image->files_amount; ++i) {
        file_t *file = &image->files[i];
        if ((file->name_len == name_len) && (strcasecmp(file->name, name) == 0))
            return file;
    }
    return NULL;
}