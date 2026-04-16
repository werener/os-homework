#include "logging.h"
#include <sys/syscall.h>
#include <unistd.h>

FILE *log_file = NULL;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void close_log() {
    if (log_file) {
        fclose(log_file);
        pthread_mutex_destroy(&log_mutex);
    }
}

void write_to_log(const char *filename, const char *status) {
    pthread_mutex_lock(&log_mutex);
    
    time_t raw_time;
    struct tm *time_info;
    char timestamp[20];
    
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time_info);
    
    pid_t thread_id = syscall(SYS_gettid);
    
    fprintf(log_file, "[%s] Thread ID: %d | File: %s | Status: %s\n",
            timestamp, thread_id, filename, status);
    fflush(log_file);
    
    pthread_mutex_unlock(&log_mutex);
}

void log_custom_message(const char *message) {
    pthread_mutex_lock(&log_mutex);
    
    fprintf(log_file, "%s", message);
    fflush(log_file);
    
    pthread_mutex_unlock(&log_mutex);
}