//
// Created by atuser on 11/22/17.
//

#include <string.h>
#include <stdlib.h>
#include "small_map.h"

static void small_map_key_free(SmallMap_key* self)
{
    free(self->key);
    OBJECT_DECREF(self->data_ptr);
    free(self);
}

SmallMap* small_map_new(U32 start_size)
{
    SmallMap* out = vector_new(VECTOR_REMOVE | VECTOR_UNORDERED);
    vector_allocate_to_size(out, start_size);
    return out;
}

void small_map_insert(SmallMap* smap, const char* key, RefObject* data)
{
    SmallMap_key* ptr = malloc(sizeof(SmallMap_key));
    ptr->free = (void (*)(void*)) small_map_key_free;
    ptr->key = strdup(key);
    ptr->data_ptr = data;
    vector_add(smap, (RefObject*) ptr);
}

RefObject* small_map_get(SmallMap* smap, const char* key)
{
    for (U32 i = 0; i != smap->n; i++)
    {
        SmallMap_key* current_key = (SmallMap_key*) vector_get(smap, i);
        if (strcmp(key, current_key->key) == 0)
            return current_key->data_ptr;
    }
    return NULL;
}

RefObject* small_map_delete(SmallMap* smap, const char* key)
{
    SmallMap_key* current_key = NULL;
    for (U32 i = 0; i != smap->n; i++)
    {
        current_key = (SmallMap_key*) vector_get(smap, i);
        if (strcmp(key, current_key->key) == 0)
        {
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

RefObject* small_map_delete_index(SmallMap* smap, U32 index)
{
    if (index >= smap->n || index < 0)
        return NULL;

    SmallMap_key* current_key = (SmallMap_key*) vector_get(smap, index);
    free(current_key->key);
    vector_remove(smap, index);
    return current_key->data_ptr;
}

char* small_map_get_key(SmallMap* smap, RefObject* data)
{
    SmallMap_key* current_key = NULL;
    for (U32 i = 0; i != smap->n; i++)
    {
        current_key = (SmallMap_key*) vector_get(smap, i);
        if (data == current_key->data_ptr)
            return current_key->key;
    }
    return NULL;
}

RefObject* small_map_get_index(SmallMap* smap, U32 index)
{
    if (index >= smap->n || index < 0)
        return NULL;

    return ((SmallMap_key*) vector_get(smap, index))->data_ptr;
}

char* small_map_get_key_index(SmallMap* smap, U32 index)
{
    return ((SmallMap_key*) vector_get(smap, index))->key;
}

void small_map_foreach(SmallMap* smap, void (* f)(void*))
{
    for (int i = 0; i < smap->n; i++)
        f(small_map_get_index(smap, i));
}