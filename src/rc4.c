#include "rc4.h"

#include <errno.h>

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

long PAGE_SIZE = -1;

void swap(byte *a, byte *b) {

    *a ^= *b;
    *b ^= *a;
    *b ^= *a;
}

state_t *rc4_init(byte *key, int key_len) {
    PAGE_SIZE = sysconf(_SC_PAGESIZE);
    if (PAGE_SIZE == -1) {
        fprintf(stderr, "Unknown page size. Unable to allocate\n");
        return NULL;
    }

    state_t *state = mmap(NULL, PAGE_SIZE, PROT_WRITE | PROT_READ,
                          MAP_PRIVATE | MAP_ANONYMOUS | MAP_LOCKED, -1, 0);
    if (state == MAP_FAILED) {
        fprintf(stderr, "Error allocating a page for rc4 state\n");
        return NULL;
    }


    // Base state
    state->i = 0;
    state->j = 0;
    for (int i = 0; i < N; ++i)
        state->S[i] = (byte)i;

    // Key scheduling
    int j = 0;
    for (int i = 0; i < N; ++i) {
        j = (j + state->S[i] + key[i % key_len]) & 0xFF;
        swap(&state->S[i], &state->S[j]);
    }

    mprotect(state, PAGE_SIZE, PROT_NONE);

    return state;
}

void rc4_apply(state_t *state, byte *data, const int32_t len) {
    mprotect(state, PAGE_SIZE, PROT_WRITE | PROT_READ);
    for (int32_t n = 0; n < len; ++n) {
        state->i = (state->i + 1) & 0xFF;
        state->j = (state->j + state->S[state->i]) & 0xFF;

        swap(&state->S[state->i], &state->S[state->j]);

        byte k = state->S[(state->S[state->i] + state->S[state->j]) & 0xFF];
        data[n] ^= k;
    }
    mprotect(state, PAGE_SIZE, PROT_NONE);
}

inline void free_state(state_t *state) {
    mprotect(state, PAGE_SIZE, PROT_WRITE);
    memset(state, 0, PAGE_SIZE);
    munmap(state, PAGE_SIZE);
}
