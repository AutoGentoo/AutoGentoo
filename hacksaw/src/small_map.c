//
// Created by atuser on 11/22/17.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <autogentoo/hacksaw/tools.h>

SmallMap* small_map_new(int start_size, int increment) {
	SmallMap* out = vector_new(sizeof(void*), REMOVE | UNORDERED);
	out->increment = (size_t) increment;
	vector_allocate_to_size(out, (size_t)start_size);
	return out;
}

void small_map_insert(SmallMap* smap, char* key, void* data) {
	SmallMap_key *ptr = malloc(sizeof(SmallMap_key));
	ptr->key = strdup(key);
	ptr->data_ptr = data;
	vector_add(smap, &ptr);
}

void* small_map_get(SmallMap* smap, char* key) {
	int i;
	for (i = 0; i != smap->n; i++) {
		SmallMap_key *current_key = *(SmallMap_key**) vector_get(smap, i);
		if (strcmp(key, current_key->key) == 0)
			return current_key->data_ptr;
	}
	return NULL;
}

void* small_map_delete(SmallMap* smap, char* key) {
	int i;
	SmallMap_key* current_key = NULL;
	for (i = 0; i != smap->n; i++) {
		current_key = *(SmallMap_key**) vector_get(smap, i);
		if (strcmp(key, current_key->key) == 0) {
			free(current_key->key);
			vector_remove(smap, i);
			break;
		}
		current_key = NULL;
	}
	if (current_key != NULL)
		return current_key->data_ptr;
	return NULL;
}

void* small_map_delete_index (SmallMap* smap, int index) {
	if (index >= smap->n || index < 0)
		return NULL;
	
	SmallMap_key* current_key = *(SmallMap_key**) vector_get(smap, index);
	free(current_key->key);
	vector_remove (smap, index);
	return current_key->data_ptr;
}

void small_map_free(SmallMap* smap, int free_data) {
	int i;
	for (i = 0; i != smap->n; i++) {
		SmallMap_key* current_key = *(SmallMap_key**) vector_get(smap, i);
		free(current_key->key);
		if (free_data)
			free(current_key->data_ptr);
		free(current_key);
	}
	
	vector_free(smap);
}

char* small_map_get_key(SmallMap* smap, void* data) {
	int i;
	SmallMap_key* current_key = NULL;
	for (i = 0; i != smap->n; i++) {
		current_key = *(SmallMap_key**) vector_get(smap, i);
		if (data == current_key->data_ptr)
			return current_key->key;
	}
	return NULL;
}

void* small_map_get_index(SmallMap* smap, int index) {
	if (index >= smap->n || index < 0)
		return NULL;
	
	return (*(SmallMap_key**) vector_get(smap, index))->data_ptr;
}

void* small_map_get_key_index(SmallMap* smap, int index) {
	return (*(void***) vector_get(smap, index))[0];
}