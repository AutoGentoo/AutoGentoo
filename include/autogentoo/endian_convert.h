//
// Created by atuser on 1/13/18.
//

#ifndef AUTOGENTOO_ENDIAN_CONVERT_H
#define AUTOGENTOO_ENDIAN_CONVERT_H

#include <stdio.h>
#include <stdint.h>
#include "autogentoo.h"

typedef struct __Endian Endian;
typedef struct __EndianNode EndianNode;

typedef enum {
	ENDIAN_CHAR = 1, //!< Don't make any changes
	ENDIAN_U16 = 2,
	ENDIAN_U32 = 4,
	ENDIAN_U64 = 8
} endian_convert_t;

struct __EndianNode {
	size_t offset;
	endian_convert_t type;
};

struct __Endian {
	void* src;
	Vector* nodes;
};

uint64_t htonll(uint64_t host);

uint64_t ntohll(uint64_t network);

Endian* endian_new(void* src);

void endian_add_handler(Endian* target, size_t offset, endian_convert_t type);

void endian_to_network(Endian* target, void* dest);

void endian_to_host(Endian* target, void* dest);

void endian_free(Endian* target);

#endif //AUTOGENTOO_ENDIAN_CONVERT_H
