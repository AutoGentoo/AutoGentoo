//
// Created by atuser on 11/5/17.
//

#ifndef HACKSAW_MAP_H
#define HACKSAW_MAP_H

#include <stdio.h>
#include "string_vector.h"
#include <stdint.h>

typedef struct __Map Map;
typedef struct __MapItem MapItem;
typedef void (*free_function) (void*);

/**
 * @class __Map
 * @author atuser
 * @date 11/05/17
 * @file map.h
 * @brief Hash map structure using murmur3 hashing
 */
struct __Map {
	MapItem** hash_table; // similar to Vector position are arbitrary
	int n; // Only need if we need to resize the array
	int overlaps; // Number of key overlaps
	size_t size; // Current size of map
	size_t realloc_at; // Target size to trigger a reallocation
	double threshold; // Upper bound percent full
};

struct __MapItem {
	char* key;
	void* data;
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
uint32_t map_insert(Map* map, char* key, void* data);

/**
 * Create a new map with array size new_size
 * Threshold triggers reallocation when n >= new_size * threshold
 * @param new_size initial size of array
 * @param threshold realloc at threshold per-cent full
 * @return new array
 */
Map* map_new(size_t new_size, double threshold);

/**
 * Realloc the map and repack the keys
 * @param map map to repack
 * @param size new array size
 */
void map_realloc(Map* map, size_t size);

/**
 * Remove a key from the map
 * @param map
 * @param key
 * @return the data ptr from the key, NULL if key not found
 */
void* map_remove(Map* map, char* key);

/**
 * Get 32-bit Murmur3 hash
 * @param data source data
 * @param nbytes sizeof dataptr
 * @return 32-bit unsigned hash value
 */
uint32_t map_get_hash(const void *data, size_t nbytes); // Returns offset from hash_table[0]

/**
 * Iterate through map and find all the keys
 * @param map map to go through
 * @return a StringVector of the keys
 */
StringVector* map_all_keys(Map* map);

/**
 * Free the map and all its data
 * @param map to free
 * @param __free for each MapItem allocated, __free (item->data) will be called
 * NULL to not free data
 */
void map_free (Map* map, free_function __free);

#endif // HACKSAW_MAP_H
