#include "caesar.h"
#include <stdio.h>

static char *KEY_PAGE = NULL;
static size_t PAGE_SIZE = 0;

size_t get_psize(void) {
    return PAGE_SIZE;
}

char *get_page(void) {
    return KEY_PAGE;
}

void free_page(void) {
    if (KEY_PAGE != NULL) {
        mprotect(KEY_PAGE, PAGE_SIZE, PROT_WRITE);
        KEY_PAGE[0] = 0;
        munmap(KEY_PAGE, PAGE_SIZE);
        KEY_PAGE = NULL;
    }
}

// #define SIMULATE_SEGFAULT yes

pthread_mutex_t encryption_mutex = PTHREAD_MUTEX_INITIALIZER;
void caesar(void *src_ptr, void *target_ptr, int len) {
    pthread_mutex_lock(&encryption_mutex);
    #ifndef SIMULATE_SEGFAULT
    mprotect(get_page(), get_psize(), PROT_READ);
    #endif

    char *src = (char *)src_ptr;
    char *target = (char *)target_ptr;
    for (int i = 0; i < len; ++i) {
        char byte = src[i];
        target[i] = byte ^ KEY_PAGE[0];
    }
    
    mprotect(KEY_PAGE, PAGE_SIZE, PROT_NONE);
    pthread_mutex_unlock(&encryption_mutex);
}

int caesar_key(char key_) {
    PAGE_SIZE = sysconf(_SC_PAGESIZE);

    KEY_PAGE = mmap(
    NULL, 
    PAGE_SIZE, 
    PROT_WRITE,     
    MAP_PRIVATE | MAP_ANONYMOUS, 
    -1, 
    0
    );

    if (KEY_PAGE == MAP_FAILED) {
        KEY_PAGE = NULL;
        return 1;
    }
    
    KEY_PAGE[0] = key_;
    mprotect(KEY_PAGE, PAGE_SIZE, PROT_NONE);
    
    return 0;
}
