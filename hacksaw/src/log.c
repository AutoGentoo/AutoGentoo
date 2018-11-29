//
// Created by atuser on 10/22/17.
//

#include <stdio.h>
#include <autogentoo/hacksaw/tools/log.h>
#include <stdarg.h>

#define AUTOGENTOO_SHOW_DEBUG


static FILE* target = NULL;

void init_log() {
	target = stdout;
}

void close_log() {
	fclose(target);
}

void lset(FILE* _target) {
	target = _target;
}

#define AUTOGENTOO_LOG_PRINT(star_color) \
va_list args; \
va_start (args, format); \
fprintf(target, ANSI_BOLD star_color " * " ANSI_COLOR); \
vfprintf(target, format, args); \
va_end (args); \
fprintf(target, ANSI_RESET "\n"); \
fflush(target);

void ldinfo(char* format, ...) {
#ifdef AUTOGENTOO_SHOW_DEBUG
	AUTOGENTOO_LOG_PRINT(ANSI_GREEN);
#endif
}

void ldwarning(char* format, ...) {
#ifdef AUTOGENTOO_SHOW_DEBUG
	AUTOGENTOO_LOG_PRINT(ANSI_YELLOW);
#endif
}

void lderror(char* format, ...) {
#ifdef AUTOGENTOO_SHOW_DEBUG
	AUTOGENTOO_LOG_PRINT(ANSI_RED);
#endif
}

void lerror(char* format, ...) {
	AUTOGENTOO_LOG_PRINT(ANSI_RED);
}

void lwarning(char* format, ...) {
	AUTOGENTOO_LOG_PRINT(ANSI_YELLOW);
}

void linfo(char* format, ...) {
	AUTOGENTOO_LOG_PRINT(ANSI_GREEN);
}
