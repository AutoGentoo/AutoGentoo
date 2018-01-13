//
// Created by atuser on 10/20/17.
//

#include <stdio.h>

#ifndef HACKSAW_HASH_H
#define HACKSAW_HASH_H

typedef enum __hash_t hash_t;
typedef struct __HashEntry HashEntry;
typedef HashEntry Hash;

enum __hash_t {
    ENTRY_MD5,
    ENTRY_SHA1,
    ENTRY_SHA256,
    ENTRY_SHA512,
    ENTRY_RMD160,
    ENTRY_WHIRLPOOL
};

struct __HashEntry {
    hash_t hash_type;
    char hash[512];
};

int check_hash(Hash* entry);
//int check_md5 ();

#endif //HACKSAW_HASH_H