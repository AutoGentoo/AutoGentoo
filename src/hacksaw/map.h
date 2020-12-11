//
// Created by atuser on 11/5/17.
//

#ifndef HACKSAW_MAP_H
#define HACKSAW_MAP_H

#include <stdio.h>
#include "object.h"
#include "string_vector.h"

typedef struct Map_prv Map;
typedef struct MapItem_prv MapItem;

typedef void (* free_function)(void*);

/**
 * @class __Map
 * @author atuser
 * @date 11/05/17
 * @file map.h
 * @brief Hash map structure using murmur3 hashing
 */
struct Map_prv
{
    OBJECT_HEADER
    MapItem** hash_table; // similar to Vector position are arbitrary
    U32 n; // Only need if we need to resize the array
    U32 overlaps; // Number of key overlaps
    U64 size; // Current size of map
    U64 realloc_at; // Target size to trigger a reallocation
    F64 threshold; // Upper bound percent full
};

struct MapItem_prv
{
    char* key;
    RefObject* data;
    MapItem* next;
};

/**
 * @brief Invokes call to get_hash and returns hash_table[offset]
 * @param map Map in question
 * @param key Key to search for
 * @return Location in hash_table
 */
void* map_get(Map* map, char* key);

/**
 * @brief Inserts data pointer in to array and 
 * @param map Map in question
 * @param key Key to search for when retrieving
 * @param data Pointer to data that will be copied
 * @return returns the hash generated from the key
 */
void map_insert(Map* map, const char* key, RefObject* data);

/**
 * Create a new map with array size new_size
 * Threshold triggers reallocation when n >= new_size * threshold
 * @param new_size initial size of array
 * @param threshold realloc at threshold per-cent full
 * @return new array
 */
Map* map_new(U64 new_size, F64 threshold);

/**
 * Remove a key from the map
 * @param map
 * @param key
 * @return the data ptr from the key, NULL if key not found
 */
RefObject* map_remove(Map* map, char* key);

/**
 * Get 32-bit Murmur3 hash
 * @param data source data
 * @param nbytes sizeof dataptr
 * @return 32-bit unsigned hash value
 */
U32 map_get_hash(const void* data, U32 nbytes); // Returns offset from hash_table[0]

/**
 * Iterate through map and find all the keys
 * @param map map to go through
 * @return a StringVector of the keys
 */
StringVector* map_all_keys(Map* map);

#endif // HACKSAW_MAP_H
