#include <tools/map.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

Map* new_map (size_t new_size, int threshold) {
    Map* out = malloc (sizeof (Map));
    
    out->size = new_size;
    out->threshold = threshold;
    out->hash_table = malloc (sizeof (Key) * new_size);
    out->keys = vector_new (sizeof (char*), REMOVE | UNORDERED);
    
    out->filled = 0;
    
    return out;
}

void map_realloc (Map* map, size_t size) {
    Key* old_hash_table = map->hash_table;
    size_t old_size = map->size;
    
    map->hash_table = malloc (sizeof(Key) * size);
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
        
        map_insert(map, &key, (void**)&(old_hash_table[offset]));
        free(old_hash_table[offset].key);
    }
    
    free (old_hash_table);
}


void* map_get_value(Map* map, char* key) {
    size_t offset = get_hash (key) % map->size;
    if (map->hash_table[offset].key == 0) {
        return NULL;
    }
    while (memcmp(map->hash_table[offset].key, key, sizeof (void*)) != 0) {
        offset += sizeof(Key*);
        offset %= map->size; // Make sure it doesn't go map of bounds
    }
    
    return map->hash_table[offset].data;
};

void* map_insert (Map* map, char** key, void** data) {
    if (map->filled + map->threshold >= map->size) {
        map_realloc(map, map->size + map->threshold);
    }
    
    Key to_copy;
    to_copy.key = strdup(*key);
    to_copy.data = *data;
    
    size_t offset = get_hash (*key) % map->size;
    while (map->hash_table[offset].key != 0) { // If collided go to next
        offset += sizeof(void*);
        offset %= map->size; // Make sure it doesn't go map of bounds
    }
    memcpy (&map->hash_table[offset], &to_copy, sizeof(void*));
    map->filled++;
    return &map->hash_table[offset];
};

static const size_t init_fnv = 2166136261U;
static const size_t fnv_mult = 16777619;

size_t get_hash(char* key) {
    size_t hash = init_fnv;
    size_t i, len;
    len = strlen (key);
    for(i = 0; i != len; i++) {
        hash = hash ^ (key[i]);
        hash = hash * fnv_mult;
    }
    
    return hash;
}