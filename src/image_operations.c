#include "image_operations.h"

void list(char *img_path) {

    FILE *img_f = fopen(img_path, "rb");
    if (!img_f) {
        fprintf(stderr, "Image file %s doesn't exist", img_path);
        return;
    }
    image_t *image = get_image(img_path);
    if (image == NULL) {
        fclose(img_f);
        return;
    }
    if (image->files_amount <= 0) {
        printf("Empty image\n");
        fclose(img_f);
        return;
    }

    sort_image(image);
    for (int32_t i = 0; i < image->files_amount; ++i) {
        printf("- %s\n", image->files[i].name);
    }
    fclose(img_f);
}

void get(char *img_path, char *key, char *src_file, char *result_path) {
    FILE *img_f = fopen(img_path, "rb");
    if (!img_f) {
        fprintf(stderr, "Image file %s doesn't exist", img_path);
        return;
    }
    image_t *image = get_image(img_path);
    if (image == NULL) {
        fclose(img_f);
        return;
    }

    file_t *file = get_by_name(image, src_file);

    FILE *result_f = fopen(result_path, "wb");
    if (!result_f) {
        fprintf(stderr, "Cannot open %s", result_path);
        fclose(img_f);
        return;
    }

    // TODO: DECRYPT file->data

    if (fwrite(file->data, file->data_len, 1, result_f) != (size_t)file->data_len) {
        fprintf(stderr, "Write failed!");
    }
    fclose(img_f);
    fclose(result_f);

    (void)key;
}