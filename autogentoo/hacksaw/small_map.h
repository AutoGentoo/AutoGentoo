//
// Created by atuser on 11/22/17.
//

#ifndef HACKSAW_SMALL_MAP_H
#define HACKSAW_SMALL_MAP_H

#include <stdio.h>
#include "vector.h"

typedef struct SmallMap_key_prv SmallMap_key;

/**
 * Vector is essentailly this:
 *              key   data_ptr
 *              [0]     [1]
 * items :  |  char*   void*
 *          |   ...     ...
*/

struct SmallMap_key_prv
{
    REFERENCE_OBJECT
    char* key;
    RefObject* data_ptr;
};

typedef Vector SmallMap;

SmallMap* small_map_new(U32 start_size);

void small_map_insert(SmallMap* smap, const char* key, RefObject* data);

RefObject* small_map_get(SmallMap* smap, const char* key);

RefObject* small_map_delete(SmallMap* smap, const char* key);

RefObject* small_map_delete_index(SmallMap* smap, U32 index);

char* small_map_get_key(SmallMap* smap, RefObject* data);

RefObject* small_map_get_index(SmallMap* smap, U32 index);

char* small_map_get_key_index(SmallMap* smap, U32 index);

void small_map_foreach(SmallMap* smap, void (* f)(void*));

#endif