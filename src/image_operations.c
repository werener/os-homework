#include "image_operations.h"

void list(char *img_path) {
   
    FILE *img_f = fopen(img_path, "rb");
    if (!img_f) {
        printf("Image file %s doesn't exist", img_path);
        return;
    }
    image_t *image = get_image(img_path);
    if (image == NULL)
        return;
    if (image->files_amount <= 0) {
        printf("Empty image\n");
        return;
    }

    sort_image(image);        
    for (int32_t i = 0; i < image->files_amount; ++i) {
        printf("- %s\n", image->files[i].name);
    }
}
