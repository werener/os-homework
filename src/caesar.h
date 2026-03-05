#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#pragma once


struct fileContents {
    void *src_ptr;
    void *target_ptr;
    int len;
};

void caesar(void *src_ptr, void *target_ptr, int len);
void caesar_key(char key_);
