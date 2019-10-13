//
// Created by atuser on 4/23/19.
//

#define _GNU_SOURCE

#include "package.h"
#include "portage.h"
#include "portage_log.h"
#include "manifest.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <share.h>
#include <autogentoo/hacksaw/map.h>
#include "database.h"

void package_metadata_init(Ebuild* ebuild) {
	if (ebuild->metadata_init)
		return;
	
	FILE* fp = fopen(ebuild->atom_manifest->full_path, "r");
	if (!fp) {
		plog_error("Failed to open %s", ebuild->atom_manifest->full_path);
		return;
	}
	
	for (int i = 0; i < ARCH_END; i++)
		ebuild->keywords[i] = KEYWORD_NONE;
	
	size_t name_size;
	char* name = NULL;
	
	size_t value_size;
	char* value = NULL;
	
	size_t n = 0;
	
	while(!feof(fp)) {
		name_size = getdelim(&name, &n, '=', fp);
		value_size = getdelim(&value, &n, '\n', fp);
		
		if (!name || !value)
			break;
		
		name[name_size - 1] = 0;
		value[value_size - 1] = 0;
		
		if (strcmp(name, "DEPEND") == 0)
			ebuild->depend = depend_parse(value);
		else if (strcmp(name, "RDEPEND") == 0)
			ebuild->rdepend = depend_parse(value);
		else if (strcmp(name, "PDEPEND") == 0)
			ebuild->pdepend = depend_parse(value);
		else if (strcmp(name, "BDEPEND") == 0)
			ebuild->bdepend = depend_parse(value);
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
			ebuild->use = useflag_iuse_parse(ebuild->parent->parent->parent, value);
	}
	
	ebuild->metadata_init = 1;
	fclose(fp);
}

Ebuild* package_init(Repository* repo, Manifest* category_man, Manifest* atom_man) {
	char* parsed_key;
	char* pcat = strdup(category_man->filename);
	*strchr(pcat, '/') = 0;
	asprintf(&parsed_key, "%s/%s", pcat, atom_man->filename);
	free(pcat);
	
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
	new_ebuild->category_manifest = category_man;
	new_ebuild->atom_manifest = atom_man;
	new_ebuild->feature_restrict = NULL;
	new_ebuild->ebuild_key = parsed_key;
	
	new_ebuild->metadata_init = 0;
	
	new_ebuild->category = atom_parsed->category;
	new_ebuild->pn = atom_parsed->name;
	new_ebuild->pv = strdup(atom_parsed->version->full_version);
	new_ebuild->revision = atom_parsed->revision;
	
	asprintf(&new_ebuild->pr, "r%d", atom_parsed->revision);
	
	new_ebuild->slot = NULL;
	new_ebuild->sub_slot = NULL;
	new_ebuild->version = atom_parsed->version;
	
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
	free(ptr->category);
	free(ptr->pn);
	free(ptr->pv);
	free(ptr->pr);
	free(ptr->ebuild_key);
	
	if (ptr->slot)
		free(ptr->slot);
	if (ptr->sub_slot)
		free(ptr->sub_slot);
	
	dependency_free(ptr->depend);
	dependency_free(ptr->bdepend);
	dependency_free(ptr->rdepend);
	dependency_free(ptr->pdepend);
	
	useflag_free(ptr->use);
	if (ptr->feature_restrict)
		vector_free(ptr->feature_restrict);
	
	requireduse_free(ptr->required_use);
	dependency_free(ptr->src_uri);
	
	atomversion_free(ptr->version);
	free(ptr);
}