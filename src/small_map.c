//
// Created by atuser on 11/22/17.
//

#include <stdio.h>
#include <tools/small_map.h>
#include <tools/vector.h>
#include <string.h>
#include <stdlib.h>

SmallMap* small_map_new (int start_size, int increment) {
    SmallMap* out = (SmallMap*)vector_new (sizeof (void*), REMOVE | UNORDERED);
    out->increment = increment;
    vector_allocate_to_size ((Vector*)out, start_size);
    return out;
}

void small_map_insert (SmallMap* smap, char* key, void* data) {
    void** ptr = malloc(sizeof(void*) * 2);
    char* new_str = strdup (key);
    memcpy (&ptr[0], &new_str, sizeof(char*));
    memcpy (&ptr[1], &data, sizeof(void*));
    vector_add((Vector*)smap, &ptr);
}

void* small_map_get (SmallMap* smap, char* key) {
    int i;
    void** current_key;
    for (i = 0; i != smap->n; i++) {
        current_key = *(void***)vector_get((Vector*)smap, i);
        if (strcmp (key, (char*)current_key[0]) == 0) {
            return current_key[1];
        }
    }
    return NULL;
}

void* small_map_delete (SmallMap* smap, char* key) {
    int i;
    void** current_key;
    for (i = 0; i != smap->n; i++) {
        current_key = *(void***)vector_get((Vector*)smap, i);
        if (strcmp (key, (char*)current_key[0]) == 0) {
            vector_remove(smap, i);
            break;
        }
        current_key = NULL;
    }
    if (current_key != NULL)
        return current_key[1];
    return NULL;
}

void small_map_free (SmallMap* smap, int free_data) {
    int i;
    void** current_key;
    for (i = 0; i != smap->n; i++) {
        current_key = *(void***)vector_get((Vector*)smap, i);
        free ((char*)current_key[0]);
        if (free_data) {
            free ((void*)current_key[1]);
        }
        free (current_key);
    }
    
    vector_free ((Vector*)smap);
}