#include <tools/map.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

Map* new_map (size_t new_size, int threshhold) {
    Map* out = malloc (sizeof (Map));
    out->size = new_size;
    out->threshhold = threshhold;
    out->hash_table = malloc (sizeof (void*) * out->size);
    
    out->filled = 0;
    return out;
}

void map_realloc (Map* map, size_t size) {
    map->size = size;
    map->hash_table = realloc (map->hash_table, sizeof(void*) * size);
}


void* map_get_value(Map* map, void* key) {
    unsigned long offset = get_hash (key) % map->size;
    if (map->hash_table[offset] == 0) {
        return NULL;
    }
    while (memcmp((((Key**)map->hash_table)[offset])->key, key, sizeof (void*)) != 0) {
        offset += sizeof(Key*);
        offset %= map->size; // Make sure it doesn't go map of bounds
    }
    
    return (((Key**)map->hash_table)[offset])->data;
};

void* map_insert (Map* map, void* key, void* data) {
    if (map->filled + map->threshhold >= map->size) {
        map_realloc(map, map->size + map->threshhold);
    }
    
    Key* to_copy = malloc (sizeof (Key));
    to_copy->key = key;
    to_copy->data = data;
    
    unsigned long offset = get_hash (key) % map->size;
    if (map->hash_table[offset] != 0) {        
        while (map->hash_table[offset] != 0) {
            offset += sizeof(void*);
            offset %= map->size; // Make sure it doesn't go map of bounds
        }
    }
    memcpy (&map->hash_table[offset], &to_copy, sizeof(void*));
    map->filled++;
    return &map->hash_table[offset];
};

unsigned long get_hash (void* key) {
    unsigned char digest[16];
    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, key, sizeof(void*));
    MD5_Final(digest, &context);
    return (unsigned long) digest;
}