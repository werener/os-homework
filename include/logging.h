#pragma once

#include "stdio.h"
#include <bits/pthreadtypes.h>

extern FILE *log_file;
extern pthread_mutex_t log_mutex;

void write_to_log(const char *filename, const char *status);
void close_log(void);
void log_custom_message(const char *message);
