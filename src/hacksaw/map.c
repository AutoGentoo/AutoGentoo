#include <stdlib.h>
#include <string.h>
#include "hacksaw.h"

static inline void* prv_map_insert_item(Map* map, MapItem* item);

static inline void prv_map_realloc_item(Map* map, MapItem* list);

static void prv_map_free_bucket(Map* map, MapItem* item)
{
    if (!item)
        return;

    prv_map_free_bucket(map, item->next);
    free(item->key);
    OBJECT_DECREF(item->data);
    free(item);
}

static void map_free(Map* map)
{
    for (int i = 0; i < map->size; i++)
        prv_map_free_bucket(map, map->hash_table[i]);
    free(map->hash_table);
    free(map);
}

Map* map_new(size_t new_size, F64 threshold)
{
    Map* out = malloc(sizeof(Map));

    out->free = (void (*)(void*)) map_free;
    out->size = new_size;
    out->threshold = threshold;
    out->realloc_at = (U64) (threshold * new_size);

    out->hash_table = calloc(new_size, sizeof(MapItem*));
    out->overlaps = 0;
    out->n = 0;

    return out;
}

static void prv_map_realloc_item(Map* map, MapItem* list)
{
    if (list->next)
        prv_map_realloc_item(map, list->next);
    list->next = NULL;
    prv_map_insert_item(map, list);
}

static inline void map_realloc(Map* map, U64 size)
{
    U64 old_size = map->size;
    MapItem** old_table = map->hash_table;

    map->hash_table = calloc(size, sizeof(MapItem*));
    map->size = size;
    map->realloc_at = map->size * map->threshold;

    for (U64 i = 0; i < old_size; i++)
        if (old_table[i])
            prv_map_realloc_item(map, old_table[i]);

    free(old_table);
}

StringVector* map_all_keys(Map* map)
{
    StringVector* out = string_vector_new();

    MapItem* current = NULL;
    for (U32 i = 0; i < map->size && out->n < map->n; i++)
    {
        current = map->hash_table[i];
        while (current)
        {
            string_vector_add(out, current->key);
            current = current->next;
        }
    }

    return out;
}

void* map_get(Map* map, char* key)
{
    U32 n = strlen(key);
    U32 index = map_get_hash(key, n) % map->size;

    MapItem* current = map->hash_table[index];
    while (current)
    {
        if (strcmp(current->key, key) == 0)
            return current->data;
        current = current->next;
    }

    return NULL;
}

void* map_remove(Map* map, char* key)
{
    U32 n = strlen(key);
    U32 index = map_get_hash(key, n) % map->size;

    MapItem* current = map->hash_table[index];
    MapItem* before = NULL;
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            void* out = current->data;

            if (!before)
                map->hash_table[index] = current->next;
            else
                before->next = current->next;
            free(current->key);
            free(current);

            return out;
        }

        current = current->next;
        if (!before)
            before = map->hash_table[index];
        else
            before = before->next;
    }

    return NULL;
}

static inline void* prv_map_insert_item(Map* map, MapItem* item)
{
    U32 n = strlen(item->key);
    U32 hash = map_get_hash(item->key, n);
    U32 offset = hash % map->size;

    if (map->hash_table[offset] != NULL)
        map->overlaps++;

    for (MapItem* iter = map->hash_table[offset]; iter; iter = iter->next)
    {
        if (strcmp(iter->key, item->key) == 0)
        {
            void* out = iter->data;

            iter->data = item->data;
            free(item->key);
            free(item);

            return out;
        }
    }

    item->next = map->hash_table[offset];
    map->hash_table[offset] = item;
    map->n++;

    return NULL;
}

void* map_insert(Map* map, const char* key, RefObject* data)
{
    if (map->n + 1 >= map->realloc_at)
        map_realloc(map, map->size * 2);

    MapItem* to_copy = malloc(sizeof(MapItem));
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
U32 map_get_hash(const void* data, U32 nbytes)
{
    if (data == NULL || nbytes == 0)
        return 0;

    const U32 c1 = 0xcc9e2d51;
    const U32 c2 = 0x1b873593;

    const U32 nblocks = (U32) (nbytes / 4);
    const U32* blocks = (const U32*) (data);
    const U8* tail = (const U8*) (data + (nblocks * 4));

    U32 h = 0;
    U32 k = 0;
    for (U32 i = 0; i < nblocks; i++)
    {
        k = blocks[i];

        k *= c1;
        k = (k << 15) | (k >> (32 - 15));
        k *= c2;

        h ^= k;
        h = (h << 13) | (h >> (32 - 13));
        h = (h * 5) + 0xe6546b64;
    }

    k = 0;
    switch (nbytes & 3)
    {
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
