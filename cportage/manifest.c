//
// Created by atuser on 4/28/19.
//

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <archive.h>
#include "manifest.h"
#include "portage_log.h"
#include "compress.h"
#include <string.h>
#include <fcntl.h>

static struct __manifest_type_link_t manifest_type_links[] = {
		{MANIFEST_IGNORE, "IGNORE"},
		{MANIFEST_DATA, "DATA"},
		{MANIFEST_MANIFEST, "MANIFEST"},
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
Manifest* manifest_metadata_parse_fp(FILE* fp, char* dir_path) {
	char* line;
	size_t read_size = 0;
	size_t n = 0;
	
	Manifest* out = NULL;
	Manifest* temp;
	
	while ((read_size = getline(&line, &n, fp)) > 0) {
		if (line[0] == '\n') {
			free(line);
			continue;
		}
		line[n - 1] = 0; // Delete newline
		
		char* type_buff = strtok(line, " ");
		manifest_t __type = -1;
		
		for (int i = 0; i < sizeof(manifest_type_links) / sizeof(manifest_type_links[0]); i++) {
			struct __manifest_type_link_t link = manifest_type_links[i];
			if (strcmp(link.type_str, type_buff) == 0) {
				__type = link.type;
				break;
			}
		}
		
		if (__type == -1) {
			plog_error("MANIFEST type %s not found", type_buff);
			free(line);
			return out;
		}
		
		temp = malloc(sizeof(Manifest));
		temp->type = __type;
		temp->next = out;
		out = temp;
		
		out->filename = strdup(strtok(NULL, " "));
		out->len = atoi(strtok(NULL, " "));
		
		asprintf(&out->full_path, "%s/%s", dir_path, out->filename);
		out->parent_dir = strdup(dir_path);
		
		free(line);
	}
	
	return out;
}

Manifest* manifest_metadata_parse(char* path) {
	char* manifest_path;
	asprintf(&manifest_path, "%s/Manifest.gz", path);
	FILE* fp = fread_archive(manifest_path);
	free(manifest_path);
	if (!fp)
		return NULL;
	
	Manifest* out = manifest_metadata_parse_fp(fp, path);
	
	return out;
}

void manifest_metadata_deep(Manifest* mans) {
	Manifest* current;
	
	FILE* fp;
	for (current = mans; current; current = current->next) {
		char* target_path = strdup(current->filename);
		*strchr(target_path, '/') = 0;
		int target_dir = open(current->full_path, O_RDONLY | O_CLOEXEC);
		if (target_dir < 0) {
			plog_error("Failed to open directory %s", target_path);
			free(target_path);
			return;
		}
		free(target_path);
		
		fp = fread_archive(current->full_path);
		if (!fp) {
			plog_error("Failed to open file %s", current->full_path);
			return;
		}
		current->parsed = manifest_metadata_parse_fp(fp, mans->parent_dir);
	}
}

void manifest_free_prv(Manifest* ptr) {
	free(ptr->filename);
	free(ptr->parent_dir);
	free(ptr->full_path);
	
	/*
	ManifestHash* next;
	ManifestHash* temp = ptr->hashes;
	while (temp) {
		next = temp->next;
		free(temp->type);
		free(temp->hash);
		free(temp);
		temp = next;
	}
	*/
	free(ptr);
}

void manifest_free(Manifest* ptr) {
	if (!ptr)
		return;
	
	Manifest* next = NULL;
	Manifest* temp = ptr;
	while (temp) {
		next = temp->next;
		manifest_free_prv(temp);
		temp = next;
	}
}
