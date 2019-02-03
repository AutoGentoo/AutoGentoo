//
// Created by atuser on 10/21/17.
//


#include <stdlib.h>
#include <string.h>
#include <autogentoo/hacksaw/tools/string_vector.h>

StringVector* string_vector_new() {
	return vector_new(sizeof(char*), VECTOR_ORDERED | VECTOR_REMOVE);
}

void string_vector_add(StringVector* vec, char* string) {
	if (vec->s >= (vec->n + 1))
		vector_allocate(vec);
	
	((char**) vec->ptr)[vec->n] = strdup(string);
	vec->n++;
}

void string_vector_insert(StringVector* vec, char* string, int index) {
	if (vec->s >= (vec->n + 1))
		vector_allocate(vec);
	
	if (vec->opts & VECTOR_ORDERED)
		memcpy (((char**) vec->ptr)[index + 1], ((char**) vec->ptr)[index], sizeof (char*) * (vec->n - index));
	else
		((char**) vec->ptr)[vec->n] = ((char**) vec->ptr)[index];
	
	
	((char**) vec->ptr)[index] = strdup(string);
	vec->n++;
}

void string_vector_set(StringVector* vec, char* string, int index) {
	while (vec->s >= index)
		vector_allocate(vec);
	
	*(char**)vector_get (vec, index) = string;
}

void string_vector_remove(StringVector* vec, int index) {
	free(string_vector_get(vec, index));
	vector_remove(vec, index);
}

void string_vector_split(StringVector* vec, char* string, char* delim) {
	if (string == NULL)
		return;
	char* buff = strtok(string, delim);
	
	while (buff != NULL) {
		string_vector_add(vec, buff);
		buff = strtok(NULL, delim);
	}
}

void string_vector_free(StringVector* vec) {
	int i;
	for (i = 0; i != vec->n; i++) {
		free(string_vector_get(vec, i));
	}
	free(vec->ptr);
	free(vec);
}

char* string_vector_get(StringVector* vec, int index) {
	return *(char**) vector_get(vec, index);
}
