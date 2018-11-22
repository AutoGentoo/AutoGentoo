//
// Created by atuser on 11/22/17.
//

#ifndef HACKSAW_SMALL_MAP_H
#define HACKSAW_SMALL_MAP_H

#include <stdio.h>
#include "vector.h"

typedef struct __SmallMap_key SmallMap_key;

/**
 * Vector is essentailly this:
 *              key   data_ptr
 *              [0]     [1]
 * items :  |  char*   void*
 *          |   ...     ...
*/

struct __SmallMap_key {
	char* key;
	void* data_ptr;
};

typedef Vector SmallMap;

SmallMap* small_map_new(int start_size, int increment);

void small_map_insert(SmallMap* smap, char* key, void* data);

void* small_map_get(SmallMap* smap, char* key);

void* small_map_delete(SmallMap* smap, char* key);

void* small_map_delete_index (SmallMap* smap, int index);

void small_map_free(SmallMap* smap, int free_data);

char* small_map_get_key(SmallMap* smap, void* data);

void* small_map_get_index(SmallMap* smap, int index);

void* small_map_get_key_index(SmallMap* smap, int index);

#endif