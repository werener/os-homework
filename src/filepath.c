#include "filepath.h"

char *make_copy_target(char *source_file, char *out_folder) {
    char *filename;
    char *result;
    size_t out_len, name_len;
    
    char *last_slash = strrchr(source_file, '/');
    
    
    if (last_slash == NULL) {
        filename = source_file;
        
    } else {
        filename = last_slash + 1;
    }
    
    out_len = strlen(out_folder);
    name_len = strlen(filename);
    
    result = (char*)malloc(out_len + name_len + 2);
    
    if (result == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }
    
    strcpy(result, out_folder);
    
    if (out_len > 0 && result[out_len - 1] != '/' && result[out_len - 1] != '\\') {
        strcat(result, "/");
    }
    
    strcat(result, filename);
    
    return result;
}