//
// Created by atuser on 10/20/17.
//

#ifndef HACKSAW_MANIFEST_H
#define HACKSAW_MANIFEST_H

typedef enum __manifest_t manifest_t;
typedef struct __Manifest Manifest;
typedef struct __ManifestEntry ManifestEntry;

#include <tools/vector.h>
#include <stdio.h>
#include <portage/hash.h>
#include <portage/package.h>
#include <language/share.h>

enum __manifest_t {
    EBUILD, // An ebuild file
    MISC, // Another file in the ebuild directory
    AUX, // A file in files/ directory
    DIST, // A distfile -- a file fetched as sources by the ebuild
};

struct __Manifest {
    Package* package;
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

void manifest_parse (Package* pkg);
manifest_t entry_parse (ManifestEntry* entry, char* str); // Returns the manifest_t of the entry
hash_t get_hash_type (char* hash);
manifest_t get_entry_type (char* str);

#endif //HACKSAW_MANIFEST_H
