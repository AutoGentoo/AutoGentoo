//
// Created by atuser on 1/3/20.
//

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <openssl/whrlpool.h>
#include <stdlib.h>
#include "hash.h"
#include "../portage_log.h"

char* hash_unwrap_digest(unsigned char* digest, int length) {
    char* out = malloc(length * 2 + 1);

    for (int n = 0; n < length; n++) {
        snprintf(&(out[n * 2]), length * 2, "%02x", (unsigned int) digest[n]);
    }

    return out;
}

char* openssl_hash(char* file, void* context, int length, int (* init)(void*), int (* update)(void*, void*, size_t),
                   int (* final)(unsigned char*, void*)) {
    init(context);

    FILE* fp = fopen(file, "r");
    if (!fp) {
        plog_error("Failed to get hash of %s", file);
        return NULL;
    }
    ssize_t read_size = 0;

    while ((read_size = fread(HASH_BLOCK, HASH_BLOCK_SIZE, 1, fp)) > 0) {
        update(context, HASH_BLOCK, read_size);
    }

    fclose(fp);

    unsigned char digest[length];
    final(digest, context);

    return hash_unwrap_digest(digest, length);
}

char* hash_md5(char* file) {
    MD5_CTX context;
    return openssl_hash(file, &context, MD5_DIGEST_LENGTH, (int (*)(void*)) MD5_Init,
                        (int (*)(void*, void*, size_t)) MD5_Update,
                        (int (*)(unsigned char*, void*)) MD5_Final);
}

char* hash_sha1(char* file) {
    SHA_CTX context;
    return openssl_hash(file, &context, SHA_DIGEST_LENGTH, (int (*)(void*)) SHA1_Init,
                        (int (*)(void*, void*, size_t)) SHA1_Update,
                        (int (*)(unsigned char*, void*)) SHA1_Final);
}

char* hash_sha256(char* file) {
    SHA256_CTX context;
    return openssl_hash(file, &context, SHA256_DIGEST_LENGTH, (int (*)(void*)) SHA256_Init,
                        (int (*)(void*, void*, size_t)) SHA256_Update,
                        (int (*)(unsigned char*, void*)) SHA256_Final);
}

char* hash_sha512(char* file) {
    SHA512_CTX context;
    return openssl_hash(file, &context, SHA512_DIGEST_LENGTH, (int (*)(void*)) SHA512_Init,
                        (int (*)(void*, void*, size_t)) SHA512_Update,
                        (int (*)(unsigned char*, void*)) SHA512_Final);
}

char* hash_rmd160(char* file) {
    RIPEMD160_CTX context;
    return openssl_hash(file, &context, RIPEMD160_DIGEST_LENGTH, (int (*)(void*)) RIPEMD160_Init,
                        (int (*)(void*, void*, size_t)) RIPEMD160_Update,
                        (int (*)(unsigned char*, void*)) RIPEMD160_Final);
}

char* hash_whirlpool(char* file) {
    WHIRLPOOL_CTX context;
    return openssl_hash(file, &context, WHIRLPOOL_DIGEST_LENGTH, (int (*)(void*)) WHIRLPOOL_Init,
                        (int (*)(void*, void*, size_t)) WHIRLPOOL_Update,
                        (int (*)(unsigned char*, void*)) WHIRLPOOL_Final);
}

char* hash(hash_t type, char* file) {
    for (int i = 0; i < sizeof(hash_function_links) / sizeof(hash_function_links[0]); i++) {
        if (hash_function_links[i].type == type)
            return hash_function_links[i].f(file);
    }

    return NULL;
}