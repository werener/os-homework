#include "rc4.h"

#include <errno.h>

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

long PAGE_SIZE = -1;

void swap(unsigned char *a, unsigned char *b) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

state_t *rc4_init(unsigned char *key) {
	PAGE_SIZE = sysconf(_SC_PAGESIZE);
	if (PAGE_SIZE == -1) {
		fprintf(stderr, "Unknown page size. Unable to allocate\n");
		return NULL;
	}
	int key_len = strlen((char *)key);
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
		state->S[i] = i;

	// Key scheduling
	for (int i = 0; i < N; ++i) {
		state->j = (state->j + state->S[i] + key[i % key_len]) % N;
		swap(&state->S[i], &state->S[state->j]);
	}

	mprotect(state, PAGE_SIZE, PROT_NONE);

	return state;
}

void rc4_crypt(state_t *state, unsigned char *data, const int32_t len) {
	for (int32_t n = 0; n < len; ++n) {
		mprotect(state, PAGE_SIZE, PROT_WRITE | PROT_READ);
		state->i = (state->i + 1) % 256;
		state->j = (state->j + state->S[state->i]) % 256;

		swap(&state->S[state->i], &state->S[state->j]);

		unsigned char k = state->S[(state->S[state->i] + state->S[state->j]) % 256];
		mprotect(state, PAGE_SIZE, PROT_NONE);
		data[n] ^= k;
	}
}

void free_state(state_t *state) {
    mprotect(state, PAGE_SIZE, PROT_WRITE);
    memset(state, 0, PAGE_SIZE);
    munmap(state, PAGE_SIZE);
}

void test(unsigned char *key, unsigned char *data, const int32_t len) {
	state_t *state = rc4_init(key);
	rc4_crypt(state, data, len);
	for (int i = 0; i < len; ++i) {
		printf("%c", data[i]);
	}
    free_state(state);
}