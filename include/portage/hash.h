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
    MD5,
    SHA1,
    SHA256,
    SHA512,
    RMD160,
    WHIRLPOOL
};

struct __HashEntry {
    hash_t hash_type;
    char hash[512];
};

int check_hash(Hash* entry);
//int check_md5 ();

#endif //HACKSAW_HASH_H