//
// Created by atuser on 4/28/19.
//

#ifndef AUTOGENTOO_MANIFEST_H
#define AUTOGENTOO_MANIFEST_H

#include <stdio.h>

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

#include "portage.h"

struct __Manifest {
	sha_hash gz_hash; // SHA512
	char* path;
	manifest_t type;
	size_t len;
	
	int dir; //!< Only head has this set
	
	ManifestHash* hashes;
	Manifest* parsed;
	Manifest* next;
};

struct __manifest_type_link_t {
	manifest_t type;
	char* type_str;
};

Manifest* manifest_metadata_parse_fp(FILE* fp, int dir);
Manifest* manifest_metadata_parse(char* path);
void manifest_metadata_deep(Manifest* mans);


#endif //AUTOGENTOO_MANIFEST_H
