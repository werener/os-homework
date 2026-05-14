#pragma once

#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>

extern pthread_mutex_t encryption_mutex;

size_t get_psize(void);
char *get_page(void);
void free_page(void);

void caesar(void *src_ptr, void *target_ptr, int len);
int caesar_key(char key_);
