static char KEY; 
void caesar(void *src_ptr, void *target_ptr, int len) {
    char *src = (char*) src_ptr;
    char *target = (char*) target_ptr;
    for (int i = 0; i < len; ++i) {
    char byte = src[i];
    target[i] = byte ^ KEY;
    }
}

void caesar_key(char key_) {
    KEY = key_;
}
