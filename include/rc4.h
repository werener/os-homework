#pragma once

#include <stdint.h>

#define N 256

extern long PAGE_SIZE;

typedef struct {
	unsigned char S[N];
	int i, j;
} state_t;

state_t *rc4_init(unsigned char *key);
void rc4_crypt(state_t *state, unsigned char *data, const int32_t len);
void free_state(state_t *state);

void test(unsigned char *key, unsigned char *data, int len);