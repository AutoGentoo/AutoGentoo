//
// Created by atuser on 4/23/19.
//

#define _GNU_SOURCE

#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <autogentoo/hacksaw/log.h>
#include <autogentoo/hacksaw/hacksaw.h>

static char log_time_buffer[64];
static StringVector* portage_call_stack;

void plog_init() {
	portage_call_stack = string_vector_new();
}

void plog_enter_stack(char* stack_name, ...) {
	va_list ap;
	va_start(ap, stack_name);
	char* stack_parsed = NULL;
	vasprintf(&stack_parsed, stack_name, ap);
	string_vector_add(portage_call_stack, stack_parsed);
	va_end(ap);
	free(stack_parsed);
}

void plog_exit_stack() {
	string_vector_remove(portage_call_stack, portage_call_stack->n - 1);
}

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
	va_end(ap);
}

void plog_error(char* fmt, ...) {
	int buff_error = errno;
	plog_format_time();
	printf("[cportage - %s] ERROR -- ", log_time_buffer);
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
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
	va_end(ap);
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
	fprintf(target, ANSI_BOLD ANSI_RED " * " ANSI_COLOR "PORTAGE CALL STACK:\n");
	for (int i = portage_call_stack->n - 1; i >= 0; i--) {
		fprintf(target, "%s\n", string_vector_get(portage_call_stack, i));
	}
	exit(1);
}