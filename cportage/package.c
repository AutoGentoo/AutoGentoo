//
// Created by atuser on 4/23/19.
//

#define _GNU_SOURCE

#include "package.h"
#include "portage_log.h"
#include "manifest.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <share.h>

void package_metadata_init(Ebuild* ebuild) {
	int fd = openat(ebuild->atom_manifest->dir, ebuild->atom_manifest->path, O_RDONLY);
	if (fd < 0) {
		plog_error("Failed to open %s", ebuild->atom_manifest->path);
		return;
	}
	
	FILE* fp = fdopen(fd, "r");
	
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
		else if (strcmp(name, "EAPI") == 0)
			ebuild->eapi = strdup(value);
		else if (strcmp(name, "SLOT") == 0)
			ebuild->slot = strdup(value);
		else if (strcmp(name, "REQUIRED_USE") == 0)
			ebuild->required_use = required_use_parse(value);
		else if (strcmp(name, "KEYWORDS") == 0)
			keyword_parse(ebuild->keywords, value);
	}
	
	ebuild->metadata_init = 1;
	fclose(fp);
}

Ebuild* package_init(Repository* repo, Manifest* category_man, Manifest* atom_man) {
	char* parsed_key;
	char* pcat = strdup(category_man->path);
	*strchr(pcat, '/') = 0;
	asprintf(&parsed_key, "%s/%s", pcat, atom_man->path);
	
	P_Atom* atom_parsed = atom_new(parsed_key);
	if (atom_parsed == NULL)
		return NULL;
	free(parsed_key);
	
	Package* target = map_get(repo->packages, atom_parsed->key);
	if (!target) {
		Package* new_package = malloc(sizeof(Package));
		new_package->key = strdup(atom_parsed->key);
		new_package->category = strdup(atom_parsed->category);
		new_package->name = strdup(atom_parsed->name);
		
		new_package->ebuilds = NULL;
		target = new_package;
		map_insert(repo->packages, atom_parsed->key, new_package);
	}
	
	Ebuild* new_ebuild = malloc(sizeof(Ebuild));
	
	new_ebuild->category_manifest = category_man;
	new_ebuild->atom_manifest = atom_man;
	
	new_ebuild->metadata_init = 0;
	
	new_ebuild->category = atom_parsed->category;
	new_ebuild->pn = atom_parsed->name;
	new_ebuild->pv = atom_parsed->version->full_version;
	new_ebuild->revision = atom_parsed->revision;
	
	asprintf(&new_ebuild->pr, "r%d", atom_parsed->revision);
	asprintf(&new_ebuild->pvr, "%s-%s", new_ebuild->pv, new_ebuild->pr);
	
	new_ebuild->slot = NULL;
	new_ebuild->eapi = NULL;
	new_ebuild->version = atom_parsed->version;
	
	/* Cached in the database */
	new_ebuild->depend = NULL;
	new_ebuild->bdepend = NULL;
	new_ebuild->rdepend = NULL;
	new_ebuild->pdepend = NULL;
	
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
				break;
			}
			if (!head->older) {
				head->older = new_ebuild;
				new_ebuild->newer = head;
				break;
			}
		}
	}
	
	free(atom_parsed);
	return new_ebuild;
}

#define ATOM_COMPARE_REVISION()\
if (cmp_rev == 0) \
	return 1; \
else if (cmp_rev > 0 && atom->range & ATOM_VERSION_L) \
	return 1; \
else if (cmp_rev < 0 && atom->range & ATOM_VERSION_G) \
	return 1;

int atom_match_ebuild(Ebuild* ebuild, P_Atom* atom) {
	int cmp = atom_version_compare(ebuild->version, atom->version);
	int cmp_rev = ebuild->revision - atom->revision;
	
	if (cmp == 0 && atom->range & ATOM_VERSION_E) {
		if (atom->range == ATOM_VERSION_E) {
			if (cmp_rev == 0)
				return 1;
		}
		else if (atom->range == ATOM_VERSION_REV)
			return 1;
		ATOM_COMPARE_REVISION()
	}
	
	if (cmp < 0 && atom->range & ATOM_VERSION_L) {
		return 1;
	}
	if (cmp > 0 && atom->range & ATOM_VERSION_G)
		return 1;
	
	return 0;
}

Ebuild* atom_resolve_ebuild(Repository* repo, P_Atom* atom, arch_t target) {
	Package* target_pkg = map_get(repo->packages, atom->key);
	if (!target_pkg) {
		plog_warn("Package '%s' not found", atom->key);
		return NULL;
	}
	
	Ebuild* current;
	for (current = target_pkg->ebuilds; current; current = current->older) {
		if (atom_match_ebuild(current, atom)) {
			keyword_t accept_keyword = KEYWORD_STABLE;
			if (!current->metadata_init)
				package_metadata_init(current);
			for (Keyword* keyword = target_pkg->keywords; keyword; keyword = keyword->next)
				if (atom_match_ebuild(current, keyword->atom))
					if (keyword->keywords[target] < accept_keyword)
						accept_keyword = keyword->keywords[target];
			if (current->keywords[target] >= accept_keyword)
				return current;
		}
	}
	
	return NULL;
}

DependencyTree* package_resolve_dependencies(char* atom) {
	
	DependencyTree* out = malloc(sizeof(DependencyTree));
	
	
	
	return out;
}