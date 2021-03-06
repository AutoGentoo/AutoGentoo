//
// Created by tumbar on 12/2/20.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <Python.h>
#include <execinfo.h>
#include "lut.h"
#include "map.h"

#define LUT_THRESHOLD 0.7

#define LUT_ID_GET_HASH(_i) ((_i >> LUT_HASH_SHIFT) & LUT_HASH_MASK)

static void lut_free(LUT* self)
{
    for (U32 i = 0; i < self->size; i++)
    {
        OBJECT_FREE(self->table[i]);
    }

    free(self->table);
    free(self);
}

static void lut_node_free(LUTNode* self)
{
    OBJECT_FREE(self->next);
    if (self->flags & LUT_FLAG_PYTHON)
        Py_XDECREF(self->data);
    else if (self->flags & LUT_FLAG_REFERENCE)
        OBJECT_DECREF(((RefObject*)self->data));

    free(self->key);
    free(self);
}

static inline LUTNode* lut_node_new()
{
    LUTNode* self = malloc(sizeof(LUTNode));
    self->free = (void (*)(void*)) lut_node_free;
    self->next = NULL;
    self->id = 0;
    self->flags = 0;

    self->data = 0;
    self->key = NULL;
    return self;
}

static inline LUTNode** lut_get_bucket(LUT* self, U32 hash)
{
    return &self->table[hash % self->size];
}

static inline void lut_realloc(LUT* self, U64 size)
{
    U64 old_size = self->size;
    LUTNode** old_table = self->table;

    self->table = calloc(size, sizeof(LUTNode*));
    self->size = size;
    self->realloc_at = self->size * LUT_THRESHOLD;

    U32 old_count = self->n;
    self->n = 0;

    for (U64 i = 0; i < old_size && self->n < old_count; i++)
    {
        LUTNode* current = old_table[i];
        LUTNode* next;
        while (current)
        {
            next = current->next;

            /* Break the chain so we can add
             * to the new table */
            current->next = NULL;

            /* Place this node into the new table */
            LUTNode** new_bucket = lut_get_bucket(self, LUT_ID_GET_HASH(current->id));
            if (*new_bucket)
            {
                /* Traverse to the last node in the new table */
                LUTNode* current_node = *new_bucket;
                for (; current_node->next; current_node = current_node->next);
                current_node->next = current;
            }
            else
            {
                /* No items in this bucket yet */
                *new_bucket = current;
            }

            self->n++;

            current = next;
        }
    }

    assert(self->n == old_count);
    free(old_table);
}

LUT* lut_new(U64 new_size)
{
    LUT* self = malloc(sizeof(LUT));
    self->free = (void (*)(void*)) lut_free;
    self->size = new_size;
    self->realloc_at = (U64) (LUT_THRESHOLD * new_size);
    self->table = calloc(new_size, sizeof(LUTNode*));
    self->n = 0;

    return self;
}

lut_id lut_insert(LUT* self, const char* key, U64 data, lut_flag_t flags)
{
    lut_flag_t flag = 0;
    lut_id out = lut_get_id(self, key, &flag);
    lut_insert_id(self, key, data, out, flag | flags);
    return out;
}

void lut_insert_id(LUT* self, const char* key, U64 data, lut_id id, lut_flag_t flag)
{
    if (self->n + 1 >= self->realloc_at)
        lut_realloc(self, self->size * 4);

    LUTNode* node = lut_node_new();

    /* Initialize the node */
    if (flag & LUT_FLAG_PYTHON)
        Py_XINCREF(data);
    else if (flag & LUT_FLAG_REFERENCE)
        OBJECT_INCREF(((RefObject*)data));
    node->data = data;
    node->id = id;
    node->flags = flag;
    node->key = strdup(key);

    LUTNode** bucket = lut_get_bucket(self, LUT_ID_GET_HASH(node->id));

    if (node->flags & LUT_FLAG_NOT_FOUND)
    {
        if (!*bucket)
            *bucket = node;
        else
        {
            /* Traverse to the end of the list */
            LUTNode* current_node = *bucket;
            for (; current_node->next; current_node = current_node->next);

            current_node->next = node;
        }

        self->n++;
    }
    else if (node->flags & LUT_FLAG_EXISTS)
    {
        /* This should always be non-NULL */
        /* We've already initialized this bucket (exists) */
        assert(*bucket);

        /* Find the correct node */
        LUTNode* current_node = *bucket;
        for(; current_node && current_node->id != node->id; current_node = current_node->next);
        assert(current_node);

        /* Overwrite the old reference */
        if (data != current_node->data)
        {
            if (flag & LUT_FLAG_PYTHON)
                Py_XINCREF(data);
            else if (flag & LUT_FLAG_REFERENCE)
                OBJECT_INCREF(((RefObject*)data));

            if (current_node->flags & LUT_FLAG_PYTHON)
                Py_XDECREF(current_node->data);
            else if (current_node->flags & LUT_FLAG_REFERENCE)
                OBJECT_DECREF(((RefObject*)current_node->data));

            current_node->data = data;
        }

        OBJECT_FREE(node);
        current_node->flags = flag;
    }
}

const char* lut_get_key(LUT* self, lut_id id)
{
    LUTNode** bucket = lut_get_bucket(self, LUT_ID_GET_HASH(id));
    for (LUTNode* current_node = *bucket; current_node; current_node = current_node->next)
    {
        if (id == current_node->id)
            return current_node->key;
    }

    return NULL;
}

U64 lut_get(LUT* self, lut_id id)
{
    LUTNode** bucket = lut_get_bucket(self, LUT_ID_GET_HASH(id));
    for (LUTNode* current_node = *bucket; current_node; current_node = current_node->next)
    {
        if (id == current_node->id)
            return current_node->data;
    }

    return 0;
}

lut_id lut_get_id(LUT* self, const char* key, lut_flag_t* flag)
{
    U32 hash = map_get_hash(key, strlen(key));
    U16 hash_overlap = 0;

    LUTNode** bucket = lut_get_bucket(self, hash);

    /* Search through the bucket to make sure we dont have
     * hash function overlaps */
    for (LUTNode* current_node = *bucket; current_node; current_node = current_node->next)
    {
        if (LUT_ID_GET_HASH(current_node->id) == hash)
        {
            /* Hashes and key match, short-circuit */
            if (strcmp(key, current_node->key) == 0)
            {
                if (flag)
                    *flag = LUT_FLAG_EXISTS;
                return current_node->id;
            }

            /* Hashes match but the keys are not the same */
            hash_overlap++;
        }
    }

    if (flag)
        *flag = LUT_FLAG_NOT_FOUND;

    return (((lut_id)hash << LUT_HASH_SHIFT) & LUT_HASH_MASK)
            | (((lut_id)hash_overlap << LUT_OVERLAP_SHIFT) & LUT_OVERLAP_MASK);

}