//
// Created by atuser on 4/23/19.
//

#include "portage_log.h"
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

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