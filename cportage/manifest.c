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
Vector* manifest_metadata_parse_fp(FILE* fp, char* dir_path) {
	char* line = malloc(512);
	size_t read_size = 0;
	size_t n = 512;
	
	Vector* out = vector_new(sizeof(Manifest*), VECTOR_REMOVE | VECTOR_UNORDERED);
	
	while ((read_size = getline(&line, &n, fp)) != -1) {
		if (line[0] == '\n')
			continue;
		line[read_size - 1] = 0; // Delete newline
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
			return out;
		}
		
		Manifest* temp = malloc(sizeof(Manifest));
		temp->type = __type;
		temp->parsed = NULL;
		
		temp->filename = strdup(strtok(NULL, " "));
		temp->len = atoi(strtok(NULL, " "));
		
		asprintf(&temp->full_path, "%s/%s", dir_path, temp->filename);
		temp->parent_dir = strdup(temp->full_path);
		*(strrchr(temp->parent_dir, '/')) = 0;
		vector_add(out, &temp);
	}
	
	free(line);
	fclose(fp);
	
	return out;
}

Vector* manifest_metadata_parse(char* path) {
	char* manifest_path;
	asprintf(&manifest_path, "%s/Manifest.gz", path);
	FILE* fp = fread_archive(manifest_path);
	free(manifest_path);
	if (!fp)
		return NULL;
	
	return manifest_metadata_parse_fp(fp, path);
}

void manifest_metadata_deep(Vector* mans) {
	Manifest* current;
	
	FILE* fp;
	for (int i = 0; i < mans->n; i++) {
		current = *(Manifest**)vector_get(mans, i);
		
		fp = fread_archive(current->full_path);
		if (!fp) {
			plog_error("Failed to open file %s", current->full_path);
			return;
		}
		current->parsed = manifest_metadata_parse_fp(fp, current->parent_dir);
	}
}

void manifest_free(Manifest* ptr) {
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
