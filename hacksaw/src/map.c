#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <autogentoo/hacksaw/tools.h>

Map* map_new (size_t new_size, int threshold) {
    Map* out = malloc (sizeof (Map));
    
    out->size = new_size;
    out->threshold = threshold;
    out->hash_table = calloc (sizeof (Key), new_size);
    out->keys = vector_new (sizeof (char*), REMOVE | UNORDERED);
    
    out->filled = 0;
    
    return out;
}

void map_realloc (Map* map, size_t size) {
    Key* old_hash_table = map->hash_table;
    size_t old_size = map->size;
    
    map->hash_table = calloc (sizeof(Key), size);
    map->size = size;
    
    char* key;
    size_t _continue, offset;
    
    int i;
    for (i = 0; i != map->keys->n; i++) {
        key = string_vector_get(map->keys, i);
        offset = get_hash (key) % old_size;
        _continue = 0;
        
        while (strcmp(old_hash_table[offset].key, key) != 0) {
            offset += sizeof(Key*);
            offset %= old_size; // Make sure it doesn't go map of bounds
            if (old_hash_table[offset].key == 0) { // If there is any empty slot it doesnt exist
                _continue = 1;
                break;
            }
        }
        
        if (_continue)
            continue;
        
        map_insert(map, key, (void*)&(old_hash_table[offset]));
        free(old_hash_table[offset].key);
    }
    
    free (old_hash_table);
}


void* map_get (Map* map, char* key) {
    size_t offset = get_hash (key) % map->size;
    if (map->hash_table[offset].key == 0) {
        return NULL;
    }
    while (memcmp(map->hash_table[offset].key, key, sizeof (void*)) != 0) {
        offset += sizeof(Key*);
        offset %= map->size; // Make sure it doesn't go map of bounds
        if (*(char*)(&map->hash_table[offset]) == 0) { // Empty byte
            return NULL;
        }
    }
    
    return map->hash_table[offset].data;
};

unsigned long map_insert(Map* map, char* key, void* data) {
    if (map->filled + map->threshold >= map->size) {
        map_realloc(map, map->size + map->threshold);
    }
    
    Key to_copy;
    to_copy.key = strdup(key);
    to_copy.data = data;
    
    unsigned long hash = get_hash (key);
    size_t offset = hash % map->size;
    while (map->hash_table[offset].key != 0) { // If collided go to next
        offset += sizeof(void*);
        offset %= map->size; // Make sure it doesn't go map of bounds
    }
    memcpy (&map->hash_table[offset], &to_copy, sizeof(void*));
    map->filled++;
    return hash;
};

unsigned long get_hash (char* key) {
    unsigned long hash = 0;
    int c;
    
    if(!key) {
        return hash;
    }
    while((c = *key++)) {
        hash = c + hash * 65599;
    }
    
    return hash;
}