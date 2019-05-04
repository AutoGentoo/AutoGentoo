//
// Created by atuser on 4/28/19.
//

#ifndef AUTOGENTOO_MANIFEST_H
#define AUTOGENTOO_MANIFEST_H

#include <stdio.h>
#include "portage.h"

typedef struct __Manifest Manifest;
typedef struct __ManifestHash ManifestHash;

typedef enum {
	MANIFEST_IGNORE,
	MANIFEST_DATA,
	MANIFEST_MANIFEST
} manifest_t;

struct __ManifestHash {
	char* type;
	char* hash;
	ManifestHash* next;
};

struct __Manifest {
	SHA_HASH gz_hash;
	char* path;
	manifest_t type;
	size_t len;
	
	ManifestHash* hashes;
	Manifest* next;
};

struct __manifest_type_link_t {
	manifest_t type;
	char* type_str;
};

Manifest* manifest_metadata_parse_fp(FILE* fp);
Manifest* manifest_metadata_parse(char* path);

#endif //AUTOGENTOO_MANIFEST_H
