//
// Created by atuser on 10/22/17.
//

#include <stdio.h>
#include "log.h"
#include <stdarg.h>
#include <pthread.h>


#define AUTOGENTOO_SHOW_DEBUG


static pthread_mutex_t log_mutex;
static FILE* target = NULL;

void init_log() {
    target = stdout;
    pthread_mutex_init(&log_mutex, NULL);
}

void close_log() {
    fclose(target);
    pthread_mutex_destroy(&log_mutex);
}

void lset(FILE* _target) {
    target = _target;
}

#define AUTOGENTOO_LOG_PRINT(star_color) \
pthread_mutex_lock(&log_mutex); \
va_list args; \
va_start (args, format); \
fprintf(target, ANSI_BOLD star_color " * " ANSI_COLOR); \
vfprintf(target, format, args); \
va_end (args); \
fprintf(target, ANSI_RESET "\n"); \
fflush(target); \
pthread_mutex_unlock(&log_mutex);

void ldinfo(char* format, ...) {
#ifdef AUTOGENTOO_SHOW_DEBUG
    AUTOGENTOO_LOG_PRINT(ANSI_GREEN)
#endif
}

void ldwarning(char* format, ...) {
#ifdef AUTOGENTOO_SHOW_DEBUG
    AUTOGENTOO_LOG_PRINT(ANSI_YELLOW)
#endif
}

void lderror(char* format, ...) {
#ifdef AUTOGENTOO_SHOW_DEBUG
    AUTOGENTOO_LOG_PRINT(ANSI_RED)
#endif
}

void lerror(char* format, ...) {
    AUTOGENTOO_LOG_PRINT(ANSI_RED)
}

void lwarning(char* format, ...) {
    AUTOGENTOO_LOG_PRINT(ANSI_YELLOW)
}

void linfo(char* format, ...) {
    AUTOGENTOO_LOG_PRINT(ANSI_GREEN)
}
