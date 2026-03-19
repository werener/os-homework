#pragma once
#include "stdio.h"
#include "pthread.h"

extern FILE *log_file;
extern pthread_mutex_t log_mutex;

void write_to_log(const char *filename, const char *status);
void close_log();
void log_custom_message(const char *message);
