#include <package/package.h>
#include <tools/vector.h>
#include <stdio.h>

#ifndef HACKSAW_MANIFEST_H
#define HACKSAW_MANIFEST_H

typedef enum __manifest_t manifest_t;
typedef enum __hash_t hash_t;
typedef struct __Manifest Manifest;
typedef struct __ManifestEntry ManifestEntry;
typedef struct __HashEntry HashEntry;

enum __manifest_t {
    EBUILD, // An ebuild file
    MISC, // Another file in the ebuild directory
    AUX, // A file in files/ directory
    DIST, // A distfile -- a file fetched as sources by the ebuild
};

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

struct __Manifest {
    Vector* entries;

    /* If the Manifest PGP signed use these */
    int is_signed;
    HashEntry hash;

};

struct __ManifestEntry {
    Vector* hashes;

    /* Properties */
    char filename[256];
    manifest_t type;
    size_t size;
};

void manifest_parse (Manifest* manifest, FILE* fp);
void entry_parse (ManifestEntry* entry, char* str);
hash_t get_hash_type (char* hash);
manifest_t get_entry_type (char* str);

#endif //HACKSAW_MANIFEST_H