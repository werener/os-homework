#pragma once

#include <stdint.h>

#define N 256

extern long PAGE_SIZE;
typedef unsigned char byte;

typedef struct {
    byte S[N];
    int i, j;
} state_t;

state_t *rc4_init(byte *key, int key_len);
void rc4_apply(state_t *state, byte *data, const int32_t len);
void free_state(state_t *state);
void rc4(byte *data, const int32_t len, byte *key);

void test(byte *key, byte *data, int len);