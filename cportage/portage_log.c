//
// Created by atuser on 4/23/19.
//

#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <autogentoo/hacksaw/log.h>

static char log_time_buffer[64];

void plog_format_time() {
	time_t rawtime;
	struct tm *info;
	rawtime = time(NULL);
	info = localtime(&rawtime);
	
	strftime(log_time_buffer, 64, "%c", info);
}

void plog_info(char* fmt, ...) {
	plog_format_time();
	printf("[cportage - %s] INFO -- ", log_time_buffer);
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	printf("\n");
}

void plog_error(char* fmt, ...) {
	int buff_error = errno;
	plog_format_time();
	printf("[cportage - %s] ERROR -- ", log_time_buffer);
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	printf("\n");
	if (errno != 0)
		printf("[cportage - %s] ERROR -- %s (%d)\n", log_time_buffer, strerror(buff_error), buff_error);
}

void plog_warn(char* fmt, ...) {
	plog_format_time();
	printf("[cportage - %s] WARN -- ", log_time_buffer);
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	printf("\n");
}

void portage_die(char* fmt, ...) {
	FILE* target = stderr;
	va_list args;
	va_start (args, fmt);
	fprintf(target, ANSI_BOLD ANSI_RED " * " ANSI_COLOR);
	vfprintf(target, fmt, args);
	va_end (args);
	fprintf(target, ANSI_RESET "\n");
	fprintf(target, ANSI_BOLD ANSI_RED " * " ANSI_COLOR "portage die, dependency resolution phase" ANSI_RESET "\n");
	fflush(target);
	exit(1);
}