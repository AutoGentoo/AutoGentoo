#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <autogentoo/hacksaw/tools.h>

unsigned long prv_map_insert_item (Map* map, MapItem* item);
void prv_map_realloc_item (Map* map, MapItem* list);


Map* map_new(size_t new_size, double threshold) {
	Map* out = malloc(sizeof(Map));
	
	out->size = new_size;
	out->threshold = threshold;
	out->realloc_at = (size_t)(threshold * new_size);
	
	out->hash_table = calloc(new_size, sizeof(MapItem*));
	out->n = 0;
	
	return out;
}

void prv_map_realloc_item (Map* map, MapItem* list) {
	if (list->next == NULL) {
		prv_map_insert_item (map, list);
		return;
	}
	
	prv_map_realloc_item(map, list->next);
	list->next = NULL;
}

void map_realloc(Map* map, size_t size) {
	size_t old_size = map->size;
	
	map->hash_table = realloc(map->hash_table, size * sizeof (MapItem*));
	map->size = size;
	memset(map->hash_table + old_size * sizeof (MapItem*), 0, (map->size - old_size) * sizeof (MapItem*)); // Only set the new data to NULL
	
	int i;
	for (i = 0; i < map->n; i++)
		if (map->hash_table[i] != NULL)
			prv_map_realloc_item (map, map->hash_table[i]);
}


void* map_get(Map* map, char* key) {
	size_t index = get_hash(key) % map->size;
	
	MapItem* current = map->hash_table[index];
	while (current) {
		if (strcmp (current->key, key) == 0)
			return current->data;
		current = current->next;
	}
	
	return NULL;
};

unsigned long prv_map_insert_item (Map* map, MapItem* item) {
	unsigned long hash = get_hash(item->key);
	size_t offset = hash % map->size;
	
	MapItem** current_pos = current_pos = &map->hash_table[offset];
	while (*current_pos) {
		if ((*current_pos)->next == NULL) {
			(*current_pos)->next = item;
			break;
		}
		current_pos = &(*current_pos)->next;
	}
	
	*current_pos = item;
	map->n++;
	
	return hash;
}

unsigned long map_insert(Map* map, char* key, void* data) {
	if (map->n + 1 >= map->realloc_at)
		map_realloc(map, map->size * 2);
	
	MapItem* to_copy = malloc (sizeof (MapItem));
	to_copy->key = strdup(key);
	to_copy->data = data;
	
	return prv_map_insert_item(map, to_copy);
};

unsigned long get_hash(char* key) {
	unsigned long hash = 0;
	int c;
	
	if (!key)
		return hash;
	while ((c = *key++))
		hash = c + hash * 65599;
	return hash;
}

void prv_map_free_bucket (Map* map, MapItem* item, free_function __free) {
	if (item->next != NULL)
		prv_map_free_bucket(map, item->next, __free);
	
	free(item->key);
	if (__free)
		__free (item->data);
	free(item);
}

void map_free (Map* map, free_function __free) {
	for (int i = 0; i < map->n; i++)
		if (map->hash_table[i])
			prv_map_free_bucket (map, map->hash_table[i], __free);
	free (map->hash_table);
	free (map);
}