//
// Created by atuser on 5/5/19.
//

#define _GNU_SOURCE

#include "atom.h"
#include "portage_log.h"
#include "emerge.h"
#include <string.h>
#include <stdlib.h>

P_Atom* atom_new(char* input) {
	P_Atom* out = malloc(sizeof(P_Atom));
	out->revision = 0;
	out->version = NULL;
	char* cat_splt = strchr(input, '/');
	if (!cat_splt) {
		plog_warn("Invalid atom: %s", input);
		return NULL;
	}
	*cat_splt = 0;
	
	char* rev_splt = strrchr(cat_splt + 1, '-');
	char* version_splt;
	if (rev_splt) {
		if (rev_splt[1] == 'r') {
			*rev_splt = 0;
			out->revision = atoi(rev_splt + 2);
			version_splt = strrchr(cat_splt + 1, '-');
		}
		else
			version_splt = rev_splt;
		if (version_splt) {
			if (version_splt[1] >= '0' && version_splt[1] <= '9')
				out->version = atom_version_new(version_splt + 1);
			*version_splt = 0;
		}
	}
	
	out->category = strdup(input);
	out->name = strdup(cat_splt + 1);
	out->repository = strdup(emerge_main->default_repo);
	
	asprintf(&out->key, "%s/%s", out->category, out->name);
	
	out->useflags = NULL;
	out->slot = NULL;
	out->sub_slot = NULL;
	out->sub_opts = ATOM_SLOT_IGNORE;
	out->range = ATOM_VERSION_ALL;
	out->blocks = ATOM_BLOCK_NONE;
	out->next = NULL;
	
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
	if (!parent)
		return;
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

void dependency_free(Dependency* ptr) {
	if (!ptr)
		return;
	
	Dependency* next = NULL;
	Dependency* temp = ptr;
	while (temp) {
		next = temp->next;
		if (ptr->depends == IS_ATOM)
			atom_free(ptr->atom);
		else
			free(ptr->target);
		free(ptr);
		temp = next;
	}
	
	temp = ptr->selectors;
	while (temp) {
		next = temp->next;
		if (ptr->depends == IS_ATOM)
			atom_free(ptr->atom);
		else
			free(ptr->target);
		free(ptr);
		temp = next;
	}
	
	if (ptr->depends == IS_ATOM)
		atom_free(ptr->atom);
	else
		free(ptr->target);
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
	if (use_flag)
		out->target = strdup(use_flag);
	else
		out->target = NULL;
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
		
		size_t scf_l = strlen(scf);
		size_t scs_l = strlen(scs);
		
		char scf_suf = 0;
		char scs_suf = 0;
		
		if (scf[scf_l - 1] > '9') {
			scf_suf = scf[scf_l - 1];
			scf[scf_l - 1] = 0;
		}
		if (scs[scs_l - 1] > '9') {
			scs_suf = scs[scs_l - 1];
			scs[scs_l - 1] = 0;
		}
		
		int cmp = strcmp(scf, scs);
		int cmp_suf = scf_suf - scs_suf;
		if (cmp != 0)
			return cmp;
		if (cmp_suf != 0)
			return cmp_suf;
	}
	
	return 0;
}
