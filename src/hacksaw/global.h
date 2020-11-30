//
// Created by tumbar on 11/30/20.
//

#ifndef AUTOGENTOO_GLOBAL_H
#define AUTOGENTOO_GLOBAL_H

#include <stdint.h>

/**
 * Define the global types used through the
 * code base.
 */

/* Unsigned integers */
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

/* Signed integers */
typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

/* Floating points */
typedef float F32;
typedef double F64;
typedef long double F128;

/* Architecture specific types */
#if UINTPTR_MAX == 0xffffffff /* 32-bit addressing mode */
typedef U32 PXX;
#elif UINTPTR_MAX == 0xffffffffffffffff /* 64-bit addressing mode */
typedef U64 PXX;
#else /* Not either? */
#error "This architecture is not supported"
#endif

#endif //AUTOGENTOO_GLOBAL_H
