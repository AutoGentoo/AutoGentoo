//
// Created by atuser on 4/28/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <archive.h>
#include "manifest.h"
#include "portage_log.h"
#include "compress.h"
#include <string.h>
#include <fcntl.h>

static struct __manifest_type_link_t manifest_type_links[] = {
		{MANIFEST_IGNORE, "IGNORE "},
		{MANIFEST_DATA, "DATA "},
		{MANIFEST_MANIFEST, "MANIFEST "},
};

ManifestHash* manifest_parse_hash (FILE* fp, int delim) {
	ManifestHash* out = malloc(sizeof(ManifestHash));
	
	size_t s = 0;
	if (getdelim(&out->type, &s, ' ', fp) == -1) {
		free (out);
		plog_error("Failed to read from file");
		return NULL;
	}
	out->type[s - 1] = 0;
	
	s = 0;
	if (getdelim(&out->hash, &s, delim, fp) == -1){
		free(out->type);
		free (out);
		plog_error("Failed to read from file");
		return NULL;
	}
	out->hash[s - 1] = 0;
	out->next = NULL;
	return out;
}

/** Parse metadata from /usr/portage/metadata */
Manifest* manifest_metadata_parse_fp(FILE* fp) {
	if (feof(fp)) {
		fclose(fp);
		return NULL;
	}
	
	char* type_buff = NULL;
	size_t s;
	if (getdelim(&type_buff, &s, ' ', fp) == -1){
		if (feof(fp)) {
			fclose(fp);
			free(type_buff);
			return NULL;
		}
		plog_error("Failed to read from file");
		free(type_buff);
		return NULL;
	}
	if (type_buff[0] == '\n') {
		free(type_buff);
		return manifest_metadata_parse_fp(fp);
	}
	
	Manifest* out = malloc(sizeof(Manifest));
	
	for (int i = 0; i < sizeof(manifest_type_links) / sizeof(manifest_type_links)[0]; i++) {
		struct __manifest_type_link_t link = manifest_type_links[i];
		if (strcmp(link.type_str, type_buff) == 0) {
			out->type = link.type;
			free(type_buff);
			type_buff = NULL;
			break;
		}
	}
	if (type_buff) {
		plog_error("MANIFEST type %s not found", type_buff);
		free(type_buff);
		free(out);
		return NULL;
	}
	
	if (out->type == MANIFEST_IGNORE) {
		getline(&out->path, &s, fp);
		out->path[s - 1] = 0; // Delete newline
		return out;
	}
	
	out->path = malloc(64);
	
	fscanf(fp, "%s %lu ", out->path, &out->len);
	out->hashes = manifest_parse_hash(fp, ' '); // BLAKE2B
	out->hashes->next = manifest_parse_hash(fp, '\n'); // SHA512
	out->next = manifest_metadata_parse_fp(fp);
	
	return out;
}

Manifest* manifest_metadata_parse(char* path) {
	int parent_dir = open(path, O_RDONLY | O_CLOEXEC);
	
	FILE* fp = fread_archive("Manifest.gz", parent_dir, NULL);
	if (!fp)
		return NULL;
	
	Manifest* out = manifest_metadata_parse_fp(fp);
	out->dir = parent_dir;
	
	return out;
}

void manifest_metadata_deep(Manifest* mans) {
	Manifest* current;
	int dir = mans->dir;
	
	FILE* fp;
	for (current = mans; current; current = current->next) {
		fp = fread_archive(current->path, dir, NULL);
		if (!fp) {
			plog_error("Failed to open file %s", current->path);
			return;
		}
		current->parsed = manifest_metadata_parse_fp(fp);
	}
}