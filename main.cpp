#include <iostream>
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

int main(int argc, char *argv[]) {
    if (argc != 5) {
        char *bin_title = argv[0];
        std::cout << "Usecase example: " << bin_title << " <lib> <key> <path_to_src> <path_to_target>\n";
        return 1;
    }
    char *lib_path = argv[1];
    char *key_arg = argv[2];
    char *src_path = argv[3];
    char *target_path = argv[4];

    /* (1) library */
    void *handle = DLOPEN(lib_path);
    if (!handle) {
        std::cerr << "Couldn't load the library: " << DLERROR() << "\n";
        return 1;
    }   

    using CaesarFunc = void(*)(void*, void*, int);
    using CaesarKeyFunc = void(*)(char);
    CaesarFunc caesar = reinterpret_cast<CaesarFunc>(DLSYM(handle, ENCRYPTION_ALIAS));
    CaesarKeyFunc caesar_key = reinterpret_cast<CaesarKeyFunc>(DLSYM(handle, KEY_ALIAS));

    const char* error = DLERROR();
    if (error) {
        std::cerr << "Error finding functions: " << error << std::endl;
        DLCLOSE(handle);
        return 1;
    }

    /* (2) key */
    int i = 0;
    char ch = key_arg[i];
    while (ch != '\0') 
        ch = key_arg[++i]; 

    if (i != 1) {
        std::cout << "Key should be exactly 1 byte - '" << key_arg << "' isn't\n";
        return 1;
    }
    else 
        caesar_key(key_arg[0]);
    
    /* (3) source_file */
    FILE* src_file = fopen(src_path, "rb");
    if (!src_file) {
        std::cout << "Couldn't open file '" << src_path << "'\n" ;
        DLCLOSE(handle);
        return 1;
    }

    /* (4) target_file */
    FILE* target_file = fopen(target_path, "wb");
    if (!target_file) {
        std::cout << "Couldn't open file '" << target_path << "'\n" ;
        DLCLOSE(handle);
        return 1;
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