#include <stdlib.h>
#include <string.h>
#include "hacksaw/hacksaw.h"
#include <stdint.h>

uint32_t prv_map_insert_item (Map* map, MapItem* item);
void prv_map_realloc_item (Map* map, MapItem* list);


Map* map_new(size_t new_size, double threshold) {
	Map* out = malloc(sizeof(Map));
	
	out->size = new_size;
	out->threshold = threshold;
	out->realloc_at = (size_t)(threshold * new_size);
	
	out->hash_table = calloc(new_size, sizeof(MapItem*));
	out->n = 0;
	out->overlaps = 0;
	
	return out;
}

void prv_map_realloc_item (Map* map, MapItem* list) {
	if (!list)
		return;
	
	prv_map_realloc_item(map, list->next);
	list->next = NULL;
	prv_map_insert_item (map, list);
}

void map_realloc(Map* map, size_t size) {
	size_t old_size = map->size;
	MapItem** old_table = map->hash_table;
	
	map->hash_table = calloc(size, sizeof (MapItem*));
	map->size = size;
	
	int i;
	for (i = 0; i < old_size; i++)
		if (old_table[i] != NULL)
			prv_map_realloc_item (map, map->hash_table[i]);
		
}

StringVector* map_all_keys(Map* map) {
	StringVector* out = string_vector_new();
	
	MapItem* current = NULL;
	int i;
	for (i = 0; i < map->size && out->n < map->n; i++) {
		current = map->hash_table[i];
		while (current) {
			string_vector_add(out, current->key);
			current = current->next;
		}
	}
	
	return out;
}

void* map_get(Map* map, char* key) {
	size_t n = strlen(key);
	size_t index = map_get_hash(key, n) % map->size;
	
	MapItem* current = map->hash_table[index];
	while (current) {
		if (strcmp (current->key, key) == 0)
			return current->data;
		current = current->next;
	}
	
	return NULL;
}

void* map_remove(Map* map, char* key) {
	size_t n = strlen(key);
	size_t index = map_get_hash(key, n) % map->size;
	
	MapItem* current = map->hash_table[index];
	while (current) {
		if (strcmp (current->key, key) == 0) {
			void* out = current->data;
			map->hash_table[index] = current->next;
			free(current->key);
			free(current);
			
			return out;
		}
		current = current->next;
	}
	
	return NULL;
}

uint32_t prv_map_insert_item (Map* map, MapItem* item) {
	size_t n = strlen(item->key);
	uint32_t hash = map_get_hash(item->key, n);
	uint32_t offset = hash % map->size;
	
	if (map->hash_table[offset] != NULL)
		map->overlaps++;
	item->next = map->hash_table[offset];
	map->hash_table[offset] = item;
	map->n++;
	
	return hash;
}

uint32_t map_insert(Map* map, char* key, void* data) {
	if (map->n + 1 >= map->realloc_at)
		map_realloc(map, map->size * 2);
	
	MapItem* to_copy = malloc (sizeof (MapItem));
	to_copy->key = strdup(key);
	to_copy->data = data;
	to_copy->next = NULL;
	
	return prv_map_insert_item(map, to_copy);
}


/**
 *
 * @param data
 * @param nbytes
 * @return
 */
uint32_t map_get_hash(const void *data, size_t nbytes) {
	if (data == NULL || nbytes == 0)
		return 0;
	
	const uint32_t c1 = 0xcc9e2d51;
	const uint32_t c2 = 0x1b873593;
	
	const int nblocks = (int)(nbytes / 4);
	const uint32_t *blocks = (const uint32_t *) (data);
	const uint8_t *tail = (const uint8_t *) (data + (nblocks * 4));
	
	uint32_t h = 0;
	
	int i;
	uint32_t k;
	for (i = 0; i < nblocks; i++) {
		k = blocks[i];
		
		k *= c1;
		k = (k << 15) | (k >> (32 - 15));
		k *= c2;
		
		h ^= k;
		h = (h << 13) | (h >> (32 - 13));
		h = (h * 5) + 0xe6546b64;
	}
	
	k = 0;
	switch (nbytes & 3) {
		case 3:
			k ^= tail[2] << 16;
		case 2:
			k ^= tail[1] << 8;
		case 1:
			k ^= tail[0];
			k *= c1;
			k = (k << 15) | (k >> (32 - 15));
			k *= c2;
			h ^= k;
	}
	
	h ^= nbytes;
	
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	
	return h;
}

void prv_map_free_bucket(Map* map, MapItem* item, free_function __free) {
	if (!item)
		return;
	
	prv_map_free_bucket(map, item->next, __free);
	free(item->key);
	if (__free)
		__free (item->data);
	free(item);
	//map->n--;
}

void map_free (Map* map, free_function __free) {
	for (int i = 0; i < map->size; i++)
		prv_map_free_bucket(map, map->hash_table[i], __free);
	free (map->hash_table);
	free (map);
}