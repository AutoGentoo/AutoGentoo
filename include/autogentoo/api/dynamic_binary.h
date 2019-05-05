//
// Created by atuser on 1/31/19.
//

#ifndef AUTOGENTOO_DYNAMIC_BINARY_H
#define AUTOGENTOO_DYNAMIC_BINARY_H

#include <stdio.h>
#include <autogentoo/hacksaw/vector.h>

typedef struct __DynamicBinary DynamicBinary;
typedef union __DynamicType DynamicType;

typedef enum {
	DYNAMIC_BIN_OK,
	DYNAMIC_BIN_ETYPE,
	DYNAMIC_BIN_EBIN /* Binary added using dynamic_binary_add */
} dynamic_bin_t;

struct array_node {
	struct array_node* parent;
	char* template_start;
	char* template_end;
	size_t size_offset;
	int size;
};

typedef enum {
	DB_ENDIAN_TARGET_NETWORK = 0x1, /* If true then we set to big_endian*/
	DB_ENDIAN_INPUT_NETWORK = 0x2 /* Default set to host_endian */
} dynamic_binary_endian_t;

struct __DynamicBinary {
	size_t template_str_size;
	size_t ptr_size;
	size_t used_size;
	size_t template_used_size;
	void* ptr;
	char* template;
	char* current_template;
	
	struct array_node* array_size;
	dynamic_binary_endian_t endian;
};

union __DynamicType {
	char* string;
	int integer;
	struct {
		void* data;
		size_t n;
	} binary;
};


DynamicBinary* dynamic_binary_new(dynamic_binary_endian_t endian);
void dynamic_binary_array_start(DynamicBinary* db);
void dynamic_binary_array_next(DynamicBinary* db);
int dynamic_binary_array_end(DynamicBinary* db);
dynamic_bin_t dynamic_binary_add(DynamicBinary* db, char type, void* data);
dynamic_bin_t dynamic_binary_add_binary(DynamicBinary* db, size_t n, void* data);
dynamic_bin_t dynamic_binary_add_quick(DynamicBinary* db, char* template, DynamicType* content);
void* dynamic_binary_free(DynamicBinary* db);

#endif //AUTOGENTOO_DYNAMIC_BINARY_H
