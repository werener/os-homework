#include "caesar.h"


static char KEY; 
void caesar(struct fileContents file) {
    char *src = (char*) file.src_ptr;
    char *target = (char*) file.target_ptr;
    for (int i = 0; i < file.len; ++i) {
        char byte = src[i];
        target[i] = byte ^ KEY;
    }
}

void caesar_key(char key_) {
    KEY = key_;
}
