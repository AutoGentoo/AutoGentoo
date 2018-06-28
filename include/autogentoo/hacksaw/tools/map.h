//
// Created by atuser on 11/5/17.
//

#ifndef HACKSAW_MAP_H
#define HACKSAW_MAP_H

#include <stdio.h>
#include "string_vector.h"

typedef struct __Map Map;
typedef struct __MapItem MapItem;
typedef void (*free_function) (void*);

/**
 * @class __Map
 * @author atuser
 * @date 11/05/17
 * @file map.h
 * @brief Hash map structure
 *          Only support array of pointers
 */
struct __Map {
	MapItem** hash_table; // similar to Vector position are arbitrary
	int n; // Only need if we need to resize the array
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
unsigned long map_insert(Map* map, char* key, void* data);

/**
 * @brief Instead of generating the hash take one
 * This is slightly faster that map_get and should be used
 * if you kept the hash
 * @param map Map in question
 * @param key Target key (you still need this)
 * @param hash Hash to use
 * @return a pointer to the data
 */
void* map_get_from_hash(Map* map, char* key, unsigned long hash);

Map* map_new(size_t new_size, double threshold);

void map_realloc(Map* map, size_t size);

unsigned long get_hash(char* key); // Returns offset from hash_table[0]

/**
 * Free the map and all its data
 * @param map to free
 * @param __free for each MapItem allocated, __free (item->data) will be called
 * NULL to not free data
 */
void map_free (Map* map, free_function __free);

#endif // HACKSAW_MAP_H
