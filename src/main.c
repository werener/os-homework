#include "caesar.h"
#include "string.h"
#include "queue.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        char *bin_title = argv[0];
        printf("Usecase example: %s <path_to_src> <path_to_target> <key>\n", bin_title);
        return 1;
    }
    
    char *src_path = argv[1];
    char *target_path = argv[2];
    char *key_arg = argv[3];

    /* (2) key */
    if (strlen(key_arg) != 1) {
        fprintf(stderr, "Key should be exactly 1 byte - '%s' isn't\n", key_arg);
        return 1;
    }
    caesar_key(key_arg[0]);
    
    /* (3) source_file */
    FILE* src_file = fopen(src_path, "rb");
    if (!src_file) 
        printf("Couldn't open file '%s'\n", src_path);

    /* (4) target_file */
    FILE* target_file = fopen(target_path, "wb");
    if (!target_file) {
        printf("Couldn't open file '%s'\n", target_path);
        fclose(src_file);
    }

    //  get source data
    fseek(src_file, 0, SEEK_END);
    long size = ftell(src_file);
    rewind(src_file); 

    void *src = malloc(size);
    fread(src, 1, size, src_file);

    //  get target data
    void *target = malloc(size);

    caesar(src, target, size);
    fwrite(target, 1, size, target_file);    //  {size} chunks with 1 byte
    

    free(src);
    free(target);
    fclose(src_file);
    fclose(target_file);
    return 0;
}