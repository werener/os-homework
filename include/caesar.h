#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>


size_t get_psize();
char *get_page();
void free_page();

void caesar(void *src_ptr, void *target_ptr, int len);
int caesar_key(char key_);
