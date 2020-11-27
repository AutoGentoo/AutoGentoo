//
// Created by atuser on 4/28/19.
//

#ifndef AUTOGENTOO_MANIFEST_H
#define AUTOGENTOO_MANIFEST_H

#include <stdio.h>
#include <autogentoo/hacksaw/vector.h>
#include <autogentoo/hacksaw/hacksaw.h>
#include "../constants.h"


typedef enum {
    MANIFEST_INVALID,
    MANIFEST_EBUILD,
    MANIFEST_MISC,
    MANIFEST_AUX,
    MANIFEST_DIST,
} manifest_t;

typedef enum {
    HASH_INVALID,
    HASH_MD5,
    HASH_SHA1,
    HASH_SHA256,
    HASH_SHA512,
    HASH_RMD160,
    HASH_WHIRLPOOL
} hash_t;

struct __ManifestHash {
    hash_t type;
    char* hash;
};

/** From https://wiki.gentoo.org/wiki/Repository_format/package/Manifest
 * <type> <filename> <size> <hash-type> <hash> [<hash-type> <hash> ...] */
struct __ManifestEntry {
    char* filename;
    int size;
    manifest_t type;

    Vector* hashes;
};

struct __Manifest {
    char* parent_dir; //!< [repo->location]/[category]/[package]
    Package* parent;

    SmallMap* ebuild;
    SmallMap* aux;
    SmallMap* dist;
    SmallMap* misc;
};

Manifest* manifest_new(Package* parent);

ManifestEntry* manifest_get(Manifest* manifest, manifest_t type, char* filename);

int manifest_verify(Manifest* manifest, manifest_t type, char* filename);

void manifest_free(Manifest* manifest);

void manifest_hash_free(ManifestHash* mh);

void manifest_entry_free(ManifestEntry* me);

#endif //AUTOGENTOO_MANIFEST_H
