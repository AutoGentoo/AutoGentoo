//
// Created by atuser on 10/22/17.
//

#include <stdlib.h>
#include <string.h>
#include <autogentoo/hacksaw/tools.h>

String* string_new(size_t start) {
	String* out = malloc(sizeof(String));
	out->increment = HACKSAW_STRING_INCREMENT;
	out->size = start;
	out->n = 0;
	out->ptr = malloc(sizeof(char) * out->size);
	return out;
}

void string_set_from(String* dest, char* source, size_t start) {
	size_t append_size = strlen(source);
	for (; append_size + start >= dest->size; string_allocate(dest));
	
	memcpy(&dest->ptr[start], source, append_size);
	dest->n = start + append_size;
}

void string_append(String* dest, char* str) {
	string_set_from(dest, str, dest->n);
}

void string_append_c(String* dest, int c) {
	dest->ptr[dest->n] = (char) c;
	dest->n++;
}

void string_allocate(String* string) {
	string->size += string->increment;
	string->ptr = realloc(string->ptr, string->size);
}

void string_free(String* string) {
	free(string->ptr);
	free(string);
}

int string_find(char** array, char* element, size_t n) {
	int i;
	for (i = 0; i != n; i++)
		if (strcmp(element, array[i]) == 0)
			return i;
	return -1;
}

void fix_path(char* ptr) {
	size_t n = strlen(ptr);
	int i;
	for (i = 1; i != n; i++) {
		if (ptr[i] == '/' && ptr[i - 1] == '/') {
			strcpy(&ptr[i - 1], &ptr[i]);
			ptr[n - 1] = 0;
			n--;
		}
	}
}

void string_overwrite(char** dest, char* src, int dup) {
	if (*dest != NULL) {
		free(*dest);
	}
	if (dup) {
		*dest = strdup(src);
	} else {
		*dest = src;
	}
}

char* string_join(char** src, char* delim, int n) {
	String* buff = string_new(strlen(src[0]));
	
	int i;
	for (i = 0; i != n; i++) {
		string_append(buff, src[i]);
		string_append(buff, delim);
	}
	
	char* out = buff->ptr;
	
	free(buff);
	return out;
}