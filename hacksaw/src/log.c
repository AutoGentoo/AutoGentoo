//
// Created by atuser on 10/22/17.
//

#include <stdio.h>
#include <autogentoo/hacksaw/tools/log.h>
#include <stdarg.h>

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

void lerror(char* format, ...) {
	va_list args;
	va_start (args, format);
	fprintf(target, ANSI_BOLD ANSI_RED " * " ANSI_COLOR);
	vfprintf(target, format, args);
	va_end (args);
	fprintf(target, ANSI_RESET "\n");
	fflush(target);
}

void lwarning(char* format, ...) {
	va_list args;
	va_start (args, format);
	fprintf(target, ANSI_BOLD ANSI_YELLOW " * " ANSI_COLOR);
	vfprintf(target, format, args);
	va_end (args);
	fprintf(target, ANSI_RESET "\n");
	fflush(target);
}

void linfo(char* format, ...) {
	va_list args;
	va_start (args, format);
	fprintf(target, ANSI_BOLD ANSI_GREEN " * " ANSI_COLOR);
	vfprintf(target, format, args);
	va_end (args);
	fprintf(target, ANSI_RESET "\n");
	fflush(target);
}
