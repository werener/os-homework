#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
    #include <windows.h>
    #define DLOPEN(path) LoadLibrary(path)
    #define DLSYM(handle, name) GetProcAddress((HMODULE)handle, name)
    #define DLCLOSE(handle) FreeLibrary((HMODULE)handle)
    #define DLERROR() "Windows error"
#else
    #include <dlfcn.h>
    #define DLOPEN(path) dlopen(path, RTLD_LAZY)
    #define DLSYM(handle, name) dlsym(handle, name)
    #define DLCLOSE(handle) dlclose(handle)
    #define DLERROR() dlerror()
#endif

#define ENCRYPTION_ALIAS "caesar"
#define KEY_ALIAS "caesar_key"

#define dl_error DLCLOSE(handle);return 1;


int main(int argc, char *argv[]) {
    if (argc != 5) {
        char *bin_title = argv[0];
        printf("Usecase example: %s <lib> <key> <path_to_src> <path_to_target>\n", bin_title);
        return 1;
    }
    char *lib_path = argv[1];
    char *key_arg = argv[2];
    char *src_path = argv[3];
    char *target_path = argv[4];

    /* (1) library */
    void *handle = DLOPEN(lib_path);
    if (!handle) {
        fprintf(stderr, "Couldn't load the library: %s\n", DLERROR());
        return 1;
    }   
    void (*caesar)(void*, void*, int) = DLSYM(handle, ENCRYPTION_ALIAS);
    void (*caesar_key)(char) = DLSYM(handle, KEY_ALIAS);

    #ifndef _WIN32
        const char* error = dlerror();
        if (error) {
            fprintf(stderr, "Error finding functions: %s\n", error);
            dl_error
        }
    #else
        if (!caesar || !caesar_key) {
            fprintf(stderr, "Error finding functions\n");
            dl_error
        }
    #endif

    /* (2) key */
    int i = 0;
    char ch = key_arg[i];
    while (ch != '\0') 
        ch = key_arg[++i]; 

    if (i != 1) {
        fprintf(stderr, "Key should be exactly 1 byte - '%s' isn't\n", key_arg);
        dl_error
    }
    else 
        caesar_key(key_arg[0]);
    
    /* (3) source_file */
    FILE* src_file = fopen(src_path, "rb");
    if (!src_file) {
        printf("Couldn't open file '%s'\n", src_path);
        dl_error
    }

    /* (4) target_file */
    FILE* target_file = fopen(target_path, "wb");
    if (!target_file) {
        printf("Couldn't open file '%s'\n", target_path);
        fclose(src_file);
        dl_error
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
    DLCLOSE(handle);
    return 0;


}