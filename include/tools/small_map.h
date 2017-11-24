//
// Created by atuser on 11/22/17.
//

#ifndef HACKSAW_SMALL_MAP_H
#define HACKSAW_SMALL_MAP_H

#include <stdio.h>
#include "vector.h"

/**
 * Vector has list of char*
 * Length of malloc of char* is strlen () + padding + 8
 * Last 8 bytes is the pointer to data
*/
typedef Vector SmallMap;

SmallMap* small_map_new (int start_size, int increment);
void small_map_insert (SmallMap* smap, char* key, void* data);
void* small_map_get (SmallMap* smap, char* key);
void small_map_free (SmallMap* smap, int free_data);

#endif