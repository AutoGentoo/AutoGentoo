//
// Created by atuser on 1/3/20.
//

#ifndef AUTOGENTOO_HASH_H
#define AUTOGENTOO_HASH_H

#include "manifest.h"

#define HASH_BLOCK_SIZE 512

char HASH_BLOCK[HASH_BLOCK_SIZE];

char* hash_md5(char* file);

char* hash_sha1(char* file);

char* hash_sha256(char* file);

char* hash_sha512(char* file);

char* hash_rmd160(char* file);

char* hash_whirlpool(char* file);

static struct {
    hash_t type;

    char* (* f)(char*);
} hash_function_links[] = {
        {HASH_MD5,       hash_md5},
        {HASH_SHA1,      hash_sha1},
        {HASH_SHA256,    hash_sha256},
        {HASH_SHA512,    hash_sha512},
        {HASH_RMD160,    hash_rmd160},
        {HASH_WHIRLPOOL, hash_whirlpool},
};

char* hash(hash_t type, char* file);

#endif //AUTOGENTOO_HASH_H
