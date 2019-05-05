//
// Created by atuser on 4/23/19.
//

#define _GNU_SOURCE

#include "package.h"
#include "portage_log.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>


P_Atom* atom_new(char* cat, char* name) {
	P_Atom* out = malloc(sizeof(P_Atom));
	out->category = strdup(cat);
	out->name = strdup(name);
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
	AtomVersion* parent = malloc (sizeof(AtomVersion));
	AtomVersion* current_node = parent;
	
	char* buf = strtok(version_str, "._-");
	while (buf) {
		char* prefix_splt = strpbrk(buf, "0123456789");
		size_t prefix_len;
		if (prefix_splt == NULL)
			prefix_len = strlen(buf);
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
			if (current_node->prefix == -1) {
				char* ebuf = strndup(buf, prefix_len);
				plog_warn("Invalid version prefix: '%s'", ebuf);
				free(ebuf);
			}
		}
		
		current_node->v = strdup(&buf[prefix_len]);
		if ((buf = strtok(NULL, "._-"))) {
			current_node->next = malloc(sizeof(AtomVersion));
			current_node = current_node->next;
		}
		else
			current_node->next = NULL;
	}
	
	return parent;
}

void atomnode_free(AtomVersion* parent) {
	AtomVersion* next;
	for (next = parent->next; parent; parent = next)
		free(parent->v);
}

void atomflag_free(AtomFlag* parent) {
	AtomFlag* next;
	for (next = parent->next; parent; parent = next)
		free(parent->name);
}

void atom_free(P_Atom* ptr) {
	atomnode_free(ptr->version);
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

Ebuild* package_init(Repository* repo, char* category, char* atom, char* hash) {
	category = strdup(category);
	atom = strdup(atom);
	*strchr(category, '/') = 0;
	
	
	char* name_splt = strrchr(atom, '-');
	char* rev_splt = NULL;
	if (!name_splt) {
		errno = EINVAL;
		plog_error("Invalid atom %s", atom);
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
	
	return new_ebuild;
}
