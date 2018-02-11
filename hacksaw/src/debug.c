//
// Created by atuser on 10/22/17.
//

#include <stddef.h>
#include <autogentoo/hacksaw/tools.h>

void print_bin(void* ptr, int n, size_t size) {
	int i;
	for (i = 0; i != (size * n); i++) {
		int j;
		for (j = 0; j < 8; j++) {
			printf("%d", !!((((char*) ptr)[i] << j) & 0x80));
		}
		
		if (!((i + 1) % size)) {
			printf(" ");
		}
	}
	printf("\n");
}

void print_vec(Vector* vec) {
	print_bin(vec->ptr, vec->n, vec->size);
	fflush(stdout);
}

void print_string_vec(StringVector* vec) {
	int i;
	for (i = 0; i != vec->n; i++) {
		printf("%p (%s) ", string_vector_get(vec, i), string_vector_get(vec, i));
	}
	printf("\n");
}

void print_config_variable(ConfVariable* var) {
	printf("%s = %s\n", var->identifier, var->value);
}

void print_config(Conf* config) {
	int i;
	for (i = 0; i != config->default_variables->n; i++) {
		print_config_variable(vector_get(config->default_variables, i));
	}
	for (i = 0; i != config->sections->n; i++) {
		ConfSection* current_section = *(ConfSection**) vector_get(config->sections, i);
		printf("[%s]\n", current_section->name);
		int j;
		for (j = 0; j != current_section->variables->n; j++) {
			print_config_variable(vector_get(current_section->variables, j));
		}
	}
}
