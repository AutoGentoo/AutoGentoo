//
// Created by atuser on 1/31/19.
//

#ifndef AUTOGENTOO_DYNAMIC_BINARY_H
#define AUTOGENTOO_DYNAMIC_BINARY_H

#include <stdio.h>
#include <autogentoo/hacksaw/tools/vector.h>

typedef struct __DynamicBinary DynamicBinary;

struct array_node {
	struct array_node* parent;
	char* template_start;
	int* size_ptr;
};

struct __DynamicBinary {
	size_t template_str_size;
	size_t ptr_size;
	size_t used_size;
	size_t template_used_size;
	void* ptr;
	char* template;
	char* current_template;
	
	struct array_node* array_size;
};

DynamicBinary* dynamic_binary_new();
void dynamic_binary_array_start(DynamicBinary* db);
void dynamic_binary_array_next(DynamicBinary* db);
void dynamic_binary_array_end(DynamicBinary* db);
void dynamic_binary_add(DynamicBinary* db, char type, void* data);
void dynamic_binary_add_binary(DynamicBinary* db, size_t n, void* data);


#endif //AUTOGENTOO_DYNAMIC_BINARY_H
