//
// Created by atuser on 5/5/19.
//

#define _GNU_SOURCE

#include "atom.h"
#include "portage_log.h"
#include "emerge.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

P_Atom* atom_new(char* input) {
	P_Atom* out = malloc(sizeof(P_Atom));
	input = strdup(input);
	out->revision = 0;
	out->version = NULL;
	char* cat_splt = strchr(input, '/');
	if (!cat_splt) {
		plog_warn("Invalid atom: %s", input);
		return NULL;
	}
	*cat_splt = 0;
	char* name_ident = cat_splt + 1;
	char* ver_splt = NULL;
	
	char* last_dash = strrchr(name_ident, '-');
	int check_second = 0;
	
	if (last_dash) {
		if (isdigit(last_dash[1]))
			ver_splt = last_dash;
		if (last_dash[1] == 'r' && isdigit(last_dash[2]))
			check_second = 1;
	}
	if (check_second) {
		*last_dash = 0;
		char* second_dash = strrchr(name_ident, '-');
		if (second_dash && isdigit(second_dash[1])) {
			*second_dash = 0;
			ver_splt = second_dash;
			out->revision = atoi(last_dash+2);
		}
		else
			*last_dash = '-';
	}
	
	if (ver_splt) {
		out->version = atom_version_new(ver_splt + 1);
		*ver_splt = 0;
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
	free(input);
	
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

AtomVersion* atom_version_new(char* input) {
	char* version_str = strdup(input);
	AtomVersion* parent = NULL;
	AtomVersion* current_node = NULL;
	AtomVersion* next_node = NULL;
	
	char* saveptr;
	
	char* buf = version_str;
	char* buf_splt = strpbrk(buf, "._-");
	while (1) {
		if (buf_splt)
			*buf_splt = 0;
		
		char* prefix_splt = strpbrk(buf, "0123456789");
		size_t prefix_len;
		if (prefix_splt == NULL) { // No prefix
			prefix_len = 0;
			prefix_splt = buf;
		}
		else
			prefix_len = prefix_splt - buf;
		
		atom_version_pre_t prefix = -1;
		
		prefix = -1;
		for (int i = 0; i < sizeof(atom_prefix_links) / sizeof(atom_prefix_links[0]); i++) {
			if (prefix_len == atom_prefix_links[i].prefix_string_len) {
				prefix = atom_prefix_links[i].pre;
				break;
			}
		}
		if (prefix == -1) {
			char* ebuf = strndup(buf, prefix_len);
			plog_warn("Invalid version prefix: '%s'", ebuf);
			free(ebuf);
			free(version_str);
			return NULL;
		}
		
		next_node = malloc(sizeof(AtomVersion));
		next_node->v = strdup(prefix_splt);
		next_node->full_version = NULL;
		next_node->next = NULL;
		next_node->prefix = prefix;
		
		if (!parent)
			parent = next_node;
		else
			current_node->next = next_node;
		current_node = next_node;
		
		if (!buf_splt)
			break;
		
		buf = buf_splt + 1;
		buf_splt = strpbrk(buf, "._-");
	}
	
	free(version_str);
	parent->full_version = strdup(input);
	return parent;
}

void atomversion_free(AtomVersion* parent) {
	AtomVersion* next = NULL;
	AtomVersion* curr = parent;
	while (curr) {
		next = curr->next;
		if (curr->full_version)
			free(curr->full_version);
		free(curr->v);
		free(curr);
		curr = next;
	}
}

void atomflag_free(AtomFlag* parent) {
	AtomFlag* next = NULL;
	while (parent) {
		next = parent->next;
		free(parent->name);
		free(parent);
		parent = next;
	}
}

void atom_free(P_Atom* ptr) {
	atomversion_free(ptr->version);
	atomflag_free(ptr->useflags);
	free(ptr->key);
	
	if (ptr->slot)
		free(ptr->slot);
	if (ptr->sub_slot)
		free(ptr->sub_slot);
	free(ptr->repository);
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
		if (temp->depends == IS_ATOM)
			atom_free(temp->atom);
		if (temp->target)
			free(temp->target);
		dependency_free(temp->selectors);
		free(temp);
		temp = next;
	}
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
