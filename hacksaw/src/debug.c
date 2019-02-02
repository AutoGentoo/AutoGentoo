//
// Created by atuser on 10/22/17.
//

#include <stddef.h>
#include <autogentoo/hacksaw/tools.h>

void print_bin(void* ptr, int n, size_t size) {
	int i;
	for (i = 0; i < n; i++) {
		printf("%hhx", ((char*)ptr)[i]);
		
		if (!((i + 1) % size))
			printf(" ");
	}
	printf("\n");
}

void print_vec(Vector* vec) {
	print_bin(vec->ptr, (int)vec->n, vec->size);
	fflush(stdout);
}

void print_string_vec(StringVector* vec) {
	int i;
	for (i = 0; i != vec->n; i++) {
		printf("%p (%s) ", (void*)string_vector_get(vec, i), string_vector_get(vec, i));
	}
	printf("\n");
}
