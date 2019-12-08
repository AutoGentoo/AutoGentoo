//
// Created by atuser on 4/23/19.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#ifndef AUTOGENTOO_PORTAGE_LOG_H
#define AUTOGENTOO_PORTAGE_LOG_H

#include <stdio.h>
#include <sys/time.h>   // for gettimeofday()
#include "constants.h"

#define LOG_TARGET stdout;

void plog_init();
void plog_enter_stack(char* stack_name, ...);
void plog_exit_stack();

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

void portage_die(char* fmt, ...);
void resolved_ebuild_print(Emerge* em, ResolvedEbuild* se);

#endif //AUTOGENTOO_PORTAGE_LOG_H

#pragma clang diagnostic pop