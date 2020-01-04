//
// Created by atuser on 4/23/19.
//

#define _GNU_SOURCE

#include "package.h"
#include "portage.h"
#include "portage_log.h"
#include "ebuild/manifest.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "language/share.h"
#include <autogentoo/hacksaw/map.h>
#include "database.h"
#include "ebuild/hash.h"

void package_metadata_init(Ebuild* ebuild) {
	if (ebuild->metadata_init)
		return;
	
	char* manifest_path = NULL;
	asprintf(&manifest_path, "%s/Manifest", ebuild->path);
	
	FILE* fp = fopen(manifest_path, "r");
	free(manifest_path);
	if (!fp) {
		plog_error("Failed to open %s", manifest_path);
		return;
	}
	
	for (int i = 0; i < ARCH_END; i++)
		ebuild->keywords[i] = KEYWORD_NONE;
	
	size_t name_size;
	size_t name_size_n;
	char* name = NULL;
	
	size_t value_size;
	size_t value_size_n;
	char* value = NULL;
	
	while(!feof(fp)) {
		name_size = getdelim(&name, &name_size_n, '=', fp);
		value_size = getdelim(&value, &value_size_n, '\n', fp);
		
		if (!name || !value)
			break;
		
		name[name_size - 1] = 0;
		value[value_size - 1] = 0;
		
		if (strcmp(name, "DEPEND") == 0) {
			if (ebuild->depend)
				portage_die("DEPEND already allocated for %s", ebuild->ebuild_key);
			ebuild->depend = depend_parse(value);
		}
		else if (strcmp(name, "RDEPEND") == 0) {
			if (ebuild->rdepend)
				portage_die("RDEPEND already allocated for %s", ebuild->ebuild_key);
			ebuild->rdepend = depend_parse(value);
		}
		else if (strcmp(name, "PDEPEND") == 0) {
			if (ebuild->pdepend)
				portage_die("PDEPEND already allocated for %s", ebuild->ebuild_key);
			ebuild->pdepend = depend_parse(value);
		}
		else if (strcmp(name, "BDEPEND") == 0) {
			if (ebuild->bdepend)
				portage_die("BDEPEND already allocated for %s", ebuild->ebuild_key);
			ebuild->bdepend = depend_parse(value);
		}
		else if (strcmp(name, "SLOT") == 0) {
			char* tok = strtok(value, "/");
			ebuild->slot = strdup(tok);
			
			tok = strtok(NULL, "/");
			if (tok)
				ebuild->sub_slot = strdup(tok);
		}
		else if (strcmp(name, "REQUIRED_USE") == 0)
			ebuild->required_use = required_use_parse(value);
		else if (strcmp(name, "KEYWORDS") == 0)
			keyword_parse(ebuild->keywords, value);
		else if (strcmp(name, "IUSE") == 0)
			ebuild->use = use_iuse_parse(ebuild->parent->parent->parent, value);
	}
	
	free(value);
	free(name);
	
	ebuild->metadata_init = 1;
	fclose(fp);
}

Ebuild* package_init(Repository* repo, char* category, char* name) {
	char* parsed_key;
	asprintf(&parsed_key, "%s/%s", category, name);
	
	P_Atom* atom_parsed = atom_new(parsed_key);
	if (atom_parsed == NULL)
		return NULL;
	
	Package* target = map_get(repo->packages, atom_parsed->key);
	if (!target) {
		Package* new_package = malloc(sizeof(Package));
		new_package->parent = repo;
		new_package->key = strdup(atom_parsed->key);
		new_package->category = strdup(atom_parsed->category);
		new_package->name = strdup(atom_parsed->name);
		new_package->keywords = NULL;
		new_package->useflags = NULL;
		
		new_package->ebuilds = NULL;
		target = new_package;
		map_insert(repo->packages, atom_parsed->key, new_package);
	}
	
	Ebuild* new_ebuild = malloc(sizeof(Ebuild));
	new_ebuild->parent = target;
	new_ebuild->feature_restrict = NULL;
	new_ebuild->ebuild_key = parsed_key;
	new_ebuild->metadata_init = 0;
	new_ebuild->path = NULL;
	new_ebuild->ebuild = NULL;
	new_ebuild->pf = NULL;
	new_ebuild->cache_file = NULL;
	new_ebuild->name = strdup(atom_parsed->name);
	new_ebuild->category = strdup(category);
	new_ebuild->revision = atom_parsed->revision;
	asprintf(&new_ebuild->path, "%s/%s/%s", repo->location, new_ebuild->category, new_ebuild->name);
	
	new_ebuild->slot = NULL;
	new_ebuild->sub_slot = NULL;
	new_ebuild->version = atom_parsed->version;
	
	if (new_ebuild->revision > 0)
		asprintf(&new_ebuild->pf, "%s-%s-r%d", new_ebuild->name, new_ebuild->version->full_version, new_ebuild->revision);
	else
		asprintf(&new_ebuild->pf, "%s-%s", new_ebuild->name, new_ebuild->version->full_version);
	
	asprintf(&new_ebuild->ebuild, "%s/%s.ebuild", new_ebuild->path, new_ebuild->pf);
	asprintf(&new_ebuild->cache_file, PORTAGE_CACHE"/%s/%s", new_ebuild->category, new_ebuild->pf);
	
	new_ebuild->ebuild_md5 = hash_md5(new_ebuild->ebuild);
	if (!new_ebuild->ebuild_md5) {
		plog_error("Failed to generate MD5 for %s", new_ebuild->ebuild);
		portage_die("Hash generation failed");
	}
	
	/* Cached in the database */
	new_ebuild->depend = NULL;
	new_ebuild->bdepend = NULL;
	new_ebuild->rdepend = NULL;
	new_ebuild->pdepend = NULL;
	
	new_ebuild->required_use = NULL;
	new_ebuild->src_uri = NULL;
	
	new_ebuild->use = NULL;
	new_ebuild->older = NULL;
	new_ebuild->newer = NULL;
	
	Ebuild* head;
	if (!target->ebuilds)
		target->ebuilds = new_ebuild;
	else {
		for (head = target->ebuilds;; head = head->older) {
			int cmp = atom_version_compare(head->version, new_ebuild->version);
			if (cmp < 0 || (cmp == 0 && head->revision - new_ebuild->revision < 0) ) {
				new_ebuild->newer = head->newer;
				new_ebuild->older = head;
				head->newer = new_ebuild;
				if (head == target->ebuilds)
					target->ebuilds = new_ebuild;
				else
					new_ebuild->newer->older = new_ebuild;
				break;
			}
			if (!head->older) {
				head->older = new_ebuild;
				new_ebuild->newer = head;
				break;
			}
		}
	}
	
	free(atom_parsed->key);
	free(atom_parsed->repository);
	free(atom_parsed);
	return new_ebuild;
}


void package_free(Package* ptr) {
	free(ptr->key);
	free(ptr->name);
	free(ptr->category);
	
	Keyword* next_key = NULL;
	Keyword* curr_key = ptr->keywords;
	while (curr_key) {
		next_key = curr_key->next;
		keyword_free(curr_key);
		curr_key = next_key;
	}
	
	Ebuild* next_eb = NULL;
	Ebuild* eb = ptr->ebuilds;
	while (eb) {
		next_eb = eb->older;
		ebuild_free(eb);
		eb = next_eb;
	}
	
	free(ptr);
}

void ebuild_free(Ebuild* ptr) {
	free(ptr->name);
	free(ptr->category);
	free(ptr->ebuild_key);
	
	if (ptr->slot)
		free(ptr->slot);
	if (ptr->sub_slot)
		free(ptr->sub_slot);
	
	dependency_free(ptr->depend);
	dependency_free(ptr->bdepend);
	dependency_free(ptr->rdepend);
	dependency_free(ptr->pdepend);
	
	use_free(ptr->use);
	if (ptr->feature_restrict)
		vector_free(ptr->feature_restrict);
	
	requireduse_free(ptr->required_use);
	dependency_free(ptr->src_uri);
	
	atomversion_free(ptr->version);
	free(ptr);
}