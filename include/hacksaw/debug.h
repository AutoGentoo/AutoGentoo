//
// Created by atuser on 10/22/17.
//

#ifndef HACKSAW_DEBUG_H
#define HACKSAW_DEBUG_H

#include <stddef.h>
#include "vector.h"
#include "string_vector.h"
#include "conf.h"

void init_log_mutex() __attribute__((constructor));

void destroy_log_mutex() __attribute__((destructor));

void print_bin(void* ptr, int n, size_t size);

void print_vec(Vector* vec);

void print_string_vec(StringVector* vec);

struct __test1 {
	int a;
	int b;
};

struct __test2 {
	int a;
	int b;
	int c;
};

#endif //HACKSAW_DEBUG_H