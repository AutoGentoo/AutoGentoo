//
// Created by atuser on 1/27/19.
//

#ifndef AUTOGENTOO_TESTS_H
#define AUTOGENTOO_TESTS_H

#include <autogentoo/endian_convert.h>

#define assert_byte_32(value, __1, __2, __3, __4) \
assert_int_equal(((value) >> (8*0)) & 0xFF, (__1)); \
assert_int_equal(((value) >> (8*1)) & 0xFF, (__2)); \
assert_int_equal(((value) >> (8*2)) & 0xFF, (__3)); \
assert_int_equal(((value) >> (8*3)) & 0xFF, (__4));

#define assert_byte_64(value, __1, __2, __3, __4, __5, __6, __7, __8) \
assert_int_equal(((value) >> (8*0)) & 0xFF, (__1)); \
assert_int_equal(((value) >> (8*1)) & 0xFF, (__2)); \
assert_int_equal(((value) >> (8*2)) & 0xFF, (__3)); \
assert_int_equal(((value) >> (8*3)) & 0xFF, (__4)); \
assert_int_equal(((value) >> (8*4)) & 0xFF, (__5)); \
assert_int_equal(((value) >> (8*5)) & 0xFF, (__6)); \
assert_int_equal(((value) >> (8*6)) & 0xFF, (__7)); \
assert_int_equal(((value) >> (8*7)) & 0xFF, (__8));

void test_htonl(void** state);

#endif //AUTOGENTOO_TESTS_H
