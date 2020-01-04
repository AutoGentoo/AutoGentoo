//
// Created by atuser on 4/28/19.
//

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include "manifest.h"
#include "../portage_log.h"
#include "../package.h"
#include <string.h>
#include "../portage.h"

ManifestHash* prv_manifest_hash_parse(char* type, char* hash) {
	static struct {
		char* type_str;
		hash_t type;
	} manifest_hash_link[] = {
			{"MD5", HASH_MD5},
			{"SHA1", HASH_SHA1},
			{"SHA256", HASH_SHA256},
			{"SHA512", HASH_SHA512},
			{"RMD160", HASH_RMD160},
			{"WHIRLPOOL", HASH_WHIRLPOOL},
	};
	
	ManifestHash* out = malloc(sizeof(ManifestHash));
	out->hash = strdup(hash);
	out->type = HASH_INVALID;
	
	for (int i = 0; i < sizeof(manifest_hash_link) / sizeof(manifest_hash_link[0]); i++) {
		if (strcmp(manifest_hash_link[i].type_str, type) == 0)
			out->type = manifest_hash_link[i].type;
	}
	
	if (out->type == HASH_INVALID) {
		plog_warn("hash type '%s' not supported (skipping)", type);
		
		manifest_hash_free(out);
		return NULL;
	}
	
	return out;
}

ManifestEntry* prv_manifest_entry_new(char* type, char* filename, char* size) {
	static struct {
		char* type_str;
		manifest_t type;
	} manifest_type_link[] = {
			{"EBUILD", MANIFEST_EBUILD},
			{"MISC", MANIFEST_MISC},
			{"AUX", MANIFEST_AUX},
			{"DIST", MANIFEST_DIST},
	};
	
	ManifestEntry* out = malloc(sizeof(ManifestEntry));
	out->type = MANIFEST_INVALID;
	out->filename = strdup(filename);
	out->size = (int)strtol(size, NULL, 10);
	out->hashes = vector_new(VECTOR_ORDERED | VECTOR_REMOVE);
	
	for (int i = 0; i < sizeof(manifest_type_link) / sizeof(manifest_type_link[0]); i++) {
		if (strcmp(manifest_type_link[i].type_str, type) == 0)
			out->type = manifest_type_link[i].type;
	}
	
	if (out->type == MANIFEST_INVALID) {
		plog_warn("manifest type '%s' not supported (skipping)", type);
		
		manifest_entry_free(out);
		return NULL;
	}
	
	return out;
}

int prv_manifest_parse(Manifest* ptr) {
	char* manifest_path = NULL;
	asprintf(&manifest_path, "%s/Manifest", ptr->parent_dir);
	FILE* fp = fopen(manifest_path, "r");
	free(manifest_path);
	
	if (!fp) {
		plog_error("Failed to open '%s' Manifest", ptr->parent->key);
		return 0;
	}
	
	char* line = NULL;
	size_t line_size = 0;
	
	/* MANIFEST_INVALID, MANIFEST_EBUILD, MANIFEST_MISC, MANIFEST_AUX, MANIFEST_DIST, */
	SmallMap* small_map_maps[] = {
		NULL,
		ptr->ebuild,
		ptr->misc,
		ptr->aux,
		ptr->dist
	};
	
	ssize_t line_len = 0;
	while ((line_len = getline(&line, &line_size, fp)) > 0) {
		line[line_len - 1] = 0;
		
		char* type = strtok(line, " ");
		char* filename = strtok(NULL, " ");
		char* size = strtok(NULL, " ");
		
		ManifestEntry* me = prv_manifest_entry_new(type, filename, size);
		if (!me)
			continue;
		
		small_map_insert(small_map_maps[me->type], filename, me);
		
		char* hash_type = strtok(NULL, " ");
		char* hash = NULL;
		if (hash_type)
			hash = strtok(NULL, " ");
		
		while (hash_type) {
			ManifestHash* mh = prv_manifest_hash_parse(hash_type, hash);
			
			if (mh)
				vector_add(me->hashes, mh);
			
			hash_type = strtok(NULL, " ");
			if (hash_type)
				hash = strtok(NULL, " ");
		}
	}
	
	free(line);
	fclose(fp);
	
	return 1;
}

Manifest* manifest_new(Package* parent) {
	Manifest* out = malloc(sizeof(Manifest));
	
	out->parent = parent;
	out->ebuild = small_map_new(5);
	out->aux = small_map_new(5);
	out->dist = small_map_new(5);
	out->misc = small_map_new(5);
	
	out->parent_dir = NULL;
	asprintf(&out->parent_dir, "%s/%s", parent->parent->location, parent->key);
	
	if (!prv_manifest_parse(out)) {
		manifest_free(out);
		return NULL;
	}
	
	return out;
}

ManifestEntry* manifest_get(Manifest* manifest, manifest_t type, char* filename) {
	SmallMap* small_map_maps[] = {
			NULL,
			manifest->ebuild,
			manifest->misc,
			manifest->aux,
			manifest->dist
	};
	
	return small_map_get(small_map_maps[type], filename);
}

int manifest_verify(Manifest* manifest, manifest_t type, char* filename) {
	// TODO
	return 1;
}

void manifest_free(Manifest* manifest) {
	free(manifest->parent_dir);
	
	small_map_foreach(manifest->dist, (void (*)(void*)) manifest_entry_free);
	small_map_foreach(manifest->aux, (void (*)(void*)) manifest_entry_free);
	small_map_foreach(manifest->ebuild, (void (*)(void*)) manifest_entry_free);
	small_map_foreach(manifest->misc, (void (*)(void*)) manifest_entry_free);
	
	small_map_free(manifest->dist, 0);
	small_map_free(manifest->aux, 0);
	small_map_free(manifest->ebuild, 0);
	small_map_free(manifest->misc, 0);
	
	free(manifest);
}

void manifest_entry_free(ManifestEntry* me) {
	free(me->filename);
	vector_foreach(me->hashes, (void (*)(void*)) manifest_hash_free);
	vector_free(me->hashes);
	
	free(me);
}

void manifest_hash_free(ManifestHash* mh) {
	free(mh->hash);
	free(mh);
}