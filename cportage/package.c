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


P_Atom* atom_new(char* input) {
	P_Atom* out = malloc(sizeof(P_Atom));
	out->version = NULL;
	char* cat_splt = strchr(input, '/');
	if (!cat_splt) {
		plog_warn("Invalid atom: %s", input);
		return NULL;
	}
	
	char* rev_splt = strrchr(input, '-');
	char* version_splt;
	if (rev_splt) {
		if (rev_splt[1] == 'r') {
			*rev_splt = 0;
			version_splt = strrchr(input, '-');
			*rev_splt = '-';
		}
		else
			version_splt = rev_splt;
		if (version_splt) {
			if (version_splt[1] >= '0' && version_splt[1] <= '9')
				out->version = atom_version_new(version_splt);
			*version_splt = 0;
		}
	}
	
	*cat_splt = 0;
	out->category = strdup(input);
	out->name = strdup(cat_splt + 1);
	
	out->useflags = NULL;
	out->version = NULL;
	out->slot = NULL;
	out->sub_slot = NULL;
	out->sub_opts = ATOM_SLOT_IGNORE;
	out->range = ATOM_VERSION_ALL;
	out->blocks = ATOM_BLOCK_NONE;
	
	return out;
}

static struct __link_atom_prefix_strct {
	size_t prefix_string_len;
	atom_version_pre_t pre;
} atom_prefix_links[] = {
		{5, ATOM_PREFIX_ALPHA},
		{4, ATOM_PREFIX_BETA},
		{3, ATOM_PREFIX_PRE},
		{2, ATOM_PREFIX_RC},
		{0, ATOM_PREFIX_NONE},
		{1, ATOM_PREFIX_P},
};

AtomVersion* atom_version_new(char* version_str) {
	version_str = strdup(version_str);
	AtomVersion* parent = malloc (sizeof(AtomVersion));
	parent->full_version = strdup(version_str);
	AtomVersion* current_node = parent;
	
	char* buf = strtok(version_str, "._-");
	while (buf) {
		char* prefix_splt = strpbrk(buf, "0123456789");
		size_t prefix_len;
		if (prefix_splt == NULL) { // No prefix
			prefix_len = 0;
			prefix_splt = buf;
		}
		else
			prefix_len = prefix_splt - buf;
		current_node->prefix = -1;
		if (*buf == 'r')
			current_node->prefix = ATOM_REVISION;
		else {
			for (int i = 0; i < sizeof(atom_prefix_links) / sizeof(atom_prefix_links[0]); i++) {
				if (prefix_len == atom_prefix_links[i].prefix_string_len) {
					current_node->prefix = atom_prefix_links[i].pre;
					break;
				}
			}
		}
		if (current_node->prefix == -1) {
			char* ebuf = strndup(buf, prefix_len);
			plog_warn("Invalid version prefix: '%s'", ebuf);
			free(ebuf);
			free(version_str);
			return NULL;
		}
		
		current_node->v = strdup(prefix_splt);
		if ((buf = strtok(NULL, "._-"))) {
			current_node->next = malloc(sizeof(AtomVersion));
			current_node = current_node->next;
			current_node->full_version = NULL;
		}
		else
			current_node->next = NULL;
	}
	
	free(version_str);
	return parent;
}

void atomversion_free(AtomVersion* parent) {
	if (!parent)
		return;
	
	if (parent->next)
		atomversion_free(parent->next);
	if (parent->full_version)
		free(parent->full_version);
	
	free(parent->v);
	free(parent);
}

void atomflag_free(AtomFlag* parent) {
	AtomFlag* next;
	for (next = parent->next; parent; parent = next)
		free(parent->name);
}

void atom_free(P_Atom* ptr) {
	atomversion_free(ptr->version);
	atomflag_free(ptr->useflags);
	
	free(ptr->category);
	free(ptr->name);
	free(ptr);
}

Dependency* dependency_build_atom(P_Atom* atom) {
	Dependency* out = malloc(sizeof(Dependency));
	out->atom = atom;
	out->target = NULL;
	out->next = NULL;
	out->depends = IS_ATOM;
	out->selector = USE_NONE;
	out->selectors = NULL;
	
	return out;
}

Dependency* dependency_build_use(char* use_flag, use_select_t type, Dependency* selector) {
	Dependency* out = malloc(sizeof(Dependency));
	out->atom = NULL;
	out->target = strdup(use_flag);
	out->next = NULL;
	out->depends = HAS_DEPENDS;
	out->selector = type;
	out->selectors = selector;
	
	return out;
}

AtomFlag* atomflag_build(char* name) {
	AtomFlag* out = malloc(sizeof(AtomFlag));
	out->name = strdup(name);
	
	return out;
}


int atom_version_compare(AtomVersion* first, AtomVersion* second) {
	AtomVersion* cf = first;
	AtomVersion* cs = second;
	
	int cmp_temp;
	for (; cf && cs; cf = cf->next, cs = cs->next) {
		char* scf = cf->v;
		char* scs = cs->v;
		
		if (cf->prefix != cs->prefix)
			return cf->prefix - cs->prefix;
		int off;
		for (off = 0; scf[off] && scs[off]; off++)
			if (scf[off] != scs[off])
				return scf[off] - scs[off];
		if (scf[off] == '\0')
			return -1;
		if (scs[off] == '\0')
			return 1;
	}
	
	return 0;
}

void package_metadata_init(Ebuild* ebuild, Manifest* atom_man) {
	int fd = openat(atom_man->dir, atom_man->path, O_RDONLY);
	if (fd < 0) {
		plog_error("Failed to open %s", atom_man->path);
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
	}
	
	fclose(fp);
}

Ebuild* package_init(Repository* repo, Manifest* category_man, Manifest* atom_man) {
	char* category = strdup(category_man->path);
	char* atom = strdup(atom_man->path);
	*strchr(category, '/') = 0;
	
	char* name_splt = strrchr(atom, '-');
	char* rev_splt = NULL;
	if (!name_splt) {
		errno = EINVAL;
		plog_error("Invalid atom %s", atom);
		free(atom);
		free(category);
		return NULL;
	}
	
	if (name_splt[1] == 'r') {
		rev_splt = name_splt;
		name_splt = strrchr(name_splt - 1, '-');
	}
	
	*name_splt = 0;
	char* pkg_name = atom;
	char* pkg_version = name_splt + 1;
	
	char key[256];
	sprintf(key, "%s/%s", category, pkg_name);
	
	Package* target = map_get(repo->packages, key);
	if (!target) {
		Package* new_package = malloc(sizeof(Package));
		new_package->key = strdup(key);
		new_package->category = strdup(category);
		new_package->name = strdup(pkg_name);
		
		new_package->ebuilds = NULL;
		target = new_package;
		map_insert(repo->packages, key, new_package);
	}
	
	Ebuild* new_ebuild = malloc(sizeof(Ebuild));
	
	if (rev_splt)
		*rev_splt = 0;
	
	new_ebuild->category = strdup(category);
	new_ebuild->pn = strdup(pkg_name);
	new_ebuild->pv = strdup(pkg_version);
	
	if (rev_splt)
		new_ebuild->pr = strdup(rev_splt + 1);
	else
		new_ebuild->pr = strdup("r0");
	
	asprintf(&new_ebuild->pvr, "%s-%s", new_ebuild->pv, new_ebuild->pr);
	new_ebuild->slot = NULL;
	new_ebuild->eapi = NULL;
	new_ebuild->version = atom_version_new(new_ebuild->pvr);
	
	/* Cached in the database */
	new_ebuild->depend = NULL;
	new_ebuild->bdepend = NULL;
	new_ebuild->rdepend = NULL;
	new_ebuild->pdepend = NULL;
	
	new_ebuild->use = NULL;
	new_ebuild->older = NULL;
	new_ebuild->newer = NULL;
	
	package_metadata_init(new_ebuild, atom_man);
	
	Ebuild* head;
	if (!target->ebuilds)
		target->ebuilds = new_ebuild;
	else {
		for (head = target->ebuilds;; head = head->older) {
			if (atom_version_compare(head->version, new_ebuild->version) < 0) {
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
	
	free(category);
	free(atom);
	
	return new_ebuild;
}
