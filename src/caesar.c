#include "caesar.h"
#include <sys/mman.h>
#include <unistd.h>

static char *KEY_PAGE = NULL;
static size_t PAGE_SIZE = 0;

void caesar(void *src_ptr, void *target_ptr, int len) {
    char *src = (char *)src_ptr;
    char *target = (char *)target_ptr;
    
    for (int i = 0; i < len; ++i) {
        char byte = src[i];
        target[i] = byte ^ KEY_PAGE[0];
    }
}


int caesar_key(char key_) {

    PAGE_SIZE = sysconf(_SC_PAGESIZE);

    KEY_PAGE = mmap(
    NULL, 
    PAGE_SIZE, 
    PROT_READ | PROT_WRITE,     
    MAP_PRIVATE | MAP_ANONYMOUS, 
    -1, 
    0
    );

    if (KEY_PAGE == MAP_FAILED) {
        KEY_PAGE = NULL;
        return 1;
    }
    
    mprotect(KEY_PAGE, PAGE_SIZE, PROT_WRITE);
    KEY_PAGE[0] = key_;
    mprotect(KEY_PAGE, PAGE_SIZE, PROT_NONE);

    return 0;
}
