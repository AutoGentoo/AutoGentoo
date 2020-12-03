//
// Created by tumbar on 12/2/20.
//

#ifndef AUTOGENTOO_LUT_H
#define AUTOGENTOO_LUT_H

#include "object.h"

/**
 * Create general purpose lookup tables for
 * fast low storage overhead access.
 *
 * We can use this for high volume/fast access
 * requirements.
 *
 * The advantage over a normal map is that we can
 * improve comparison speed and access by serializing
 * the index in the map as well as the bucket.
 *
 * To get fast reverse key -> id access we need to
 * implement this using a hash map data structure.
 * The issue is that if a standard index is used as the
 * ID, when the map is resized we lose integrity.
 *
 * To fix this issue we can serialize a 64-bit integer
 * to include extra metadata include the original string's
 * 32-bit hash.
 */

typedef struct LUT_prv LUT;
typedef struct LUTNode_prv LUTNode;

#define LUT_HASH_MASK    0x00000000ffffffff
#define LUT_OVERLAP_MASK 0x0000ffff00000000

#define LUT_HASH_SHIFT 0
#define LUT_OVERLAP_SHIFT 32

typedef U64 lut_id;

typedef enum {
    LUT_FLAG_NONE,
    LUT_FLAG_EXISTS,            //!< This key already exists in the map
    LUT_FLAG_NOT_FOUND          //!< This key does not yet exist in the map
} lut_flag_t;

struct LUTNode_prv {
    OBJECT_HEADER

    lut_id id; //!< Serialized position in LUT
    char* key;
    RefObject* data;

    lut_flag_t flags; //!< Some metadata about how this node was generated
    LUTNode* next;
};

struct LUT_prv {
    OBJECT_HEADER
    LUTNode** table;
    U32 n;
    U64 size;
    U64 realloc_at;
};

LUT* lut_new(U64 new_size);
lut_id lut_insert(LUT* self, const char* key, RefObject* data);
void lut_insert_id(LUT* self, const char* key, RefObject* data, lut_id id, lut_flag_t flag);
const char* lut_get_key(LUT* self, lut_id id);
RefObject* lut_get(LUT* self, lut_id id);
lut_id lut_get_id(LUT* self, const char* key, lut_flag_t*);

#endif //AUTOGENTOO_LUT_H
