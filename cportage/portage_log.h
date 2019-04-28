//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_PORTAGE_LOG_H
#define AUTOGENTOO_PORTAGE_LOG_H

#include <stdio.h>
#include <sys/time.h>   // for gettimeofday()

#define LOG_TARGET stdout;

void plog_info(char* fmt, ...);
void plog_error(char* fmt, ...);
void plog_warn(char* fmt, ...);

#define PLOG_BENCHMARK(stmt, name) { \
	clock_t start, end; \
	double cpu_time_used; \
	start = clock(); \
	stmt; \
	end = clock(); \
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; \
	plog_info("BENCHMARK %s (%lf)", name, cpu_time_used); \
}

#endif //AUTOGENTOO_PORTAGE_LOG_H
