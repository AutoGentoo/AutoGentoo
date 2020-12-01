//
// Created by tumbar on 11/30/20.
//

#ifndef AUTOGENTOO_OBJECT_H
#define AUTOGENTOO_OBJECT_H

#include "global.h"

/**
 * Define the object header that governs
 * all objects.
 *
 * This type header will define the function that is needed
 * to free itself and all of its children.
 */

#define OBJECT_HEADER void (*free) (void*);

/* Create an object that we can cast to */
typedef struct Object_prv Object;

struct Object_prv
{
    OBJECT_HEADER
};

/* Define an object type that can where reference
 * holding is not well defined
 *
 * Here we use Python's method of reference counting
 * */

#define REFERENCE_OBJECT \
OBJECT_HEADER            \
U64 reference_count;

typedef struct RefObject_prv RefObject;

struct RefObject_prv
{
    REFERENCE_OBJECT
};

#define OBJECT_FREE(__self) if (__self) __self->free(__self)
#define OBJECT_INCREF(__self) __self->reference_count++
#define OBJECT_DECREF(__self) if (__self && --__self->reference_count) OBJECT_FREE(__self)

#endif //AUTOGENTOO_OBJECT_H
