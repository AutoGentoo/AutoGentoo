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
#include "use.h"

P_Atom* cmdline_atom_new(char* name) {
	char* cmd_temp = NULL;
	asprintf(&cmd_temp, "SEARCH/%s", name);
	P_Atom* out = atom_new(cmd_temp);
	free(cmd_temp);
	free(name);
	
	return out;
}

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
			out->revision = (int)strtol(last_dash+2, NULL, 10);
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
	out->repository = strdup(emerge_main->default_repo_name);
	
	asprintf(&out->key, "%s/%s", out->category, out->name);
	
	out->useflags = NULL;
	out->slot = NULL;
	out->sub_slot = NULL;
	out->sub_opts = ATOM_SLOT_IGNORE;
	out->range = ATOM_VERSION_ALL;
	out->blocks = ATOM_BLOCK_NONE;
	out->parent = NULL;
	out->repo_selected = ATOM_REPO_ALL;
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
		else {
			prefix_len = prefix_splt - buf;
		}
		
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
	Dependency* next = NULL;
	Dependency* temp = ptr;
	while (temp) {
		next = temp->next;
		if (temp->atom)
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
	out->atom->parent = out;
	
	return out;
}

Dependency* dependency_build_grouping(Dependency* selectors) {
	Dependency* out = malloc(sizeof(Dependency));
	out->atom = NULL;
	out->target = NULL;
	out->next = NULL;
	out->depends = HAS_DEPENDS;
	out->selector = USE_NONE;
	out->selectors = selectors;
	
	return out;
}

Dependency* dependency_build_use(char* use_flag, use_t type, Dependency* selector) {
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
	out->option = ATOM_USE_ENABLE;
	
	if (name[0] == '-') {
		out->option = ATOM_USE_DISABLE;
		name++;
	}
	
	out->name = strdup(name);
	out->def = 0;
	out->next = NULL;
	
	return out;
}

int atom_version_compare(AtomVersion* first, AtomVersion* second) {
	AtomVersion* cf = first;
	AtomVersion* cs = second;
	
	for (; cf && cs; cf = cf->next, cs = cs->next) {
		char* scf = cf->v;
		char* scs = cs->v;
		
		if (cf->prefix != cs->prefix)
			return (int)(cf->prefix - cs->prefix);
		
		size_t scf_l = strlen(scf);
		size_t scs_l = strlen(scs);
		
		int star_skip = 0;
		if (scf[scf_l - 1] == '*') {
			star_skip = 1;
			scf_l--;
		}
		else if (scs[scs_l - 1] == '*') {
			star_skip = 1;
			scs_l--;
		}
		
		char scf_suf = 0;
		char scs_suf = 0;
		
		/* 12.5.4 > 12.5b */
		if (cs->next && scf[scf_l - 1] > '9')
			return -1;
		if (cf->next && scs[scs_l - 1] > '9')
			return 1;
		
		if (scf[scf_l - 1] > '9')
			scf_suf = scf[scf_l - 1];
		if (scs[scs_l - 1] > '9')
			scs_suf = scs[scs_l - 1];
		
		int cmp_1;
		int cmp_2;
		
		if ((scf[0] != '0' || scf_l == 1) && (scs[0] != '0' || scs_l == 1)) {
			cmp_1 = (int)strtol(scf, NULL, 10);
			cmp_2 = (int)strtol(scs, NULL, 10);
		}
		else {
			int maxlength = scf_l > scs_l ? (int)scf_l : (int)scs_l;
			
			char* r1 = malloc(maxlength + 1);
			char* r2 = malloc(maxlength + 1);
			r1[maxlength] = 0;
			r2[maxlength] = 0;
			
			strncpy(r1, scf, scf_l);
			strncpy(r2, scs, scs_l);
			
			for (int i1 = scf_l; i1 < maxlength; i1++)
				r1[i1] = '0';
			for (int i2 = scs_l; i2 < maxlength; i2++)
				r2[i2] = '0';
			
			cmp_1 = (int)strtol(r1, NULL, 10);
			cmp_2 = (int)strtol(r2, NULL, 10);
			
			free(r1);
			free(r2);
		}
		
		int cmp = cmp_1 - cmp_2;
		int cmp_suf = scf_suf - scs_suf;
		if (cmp != 0)
			return cmp;
		
		if (star_skip)
			return 0;
		
		if (cmp_suf != 0)
			return cmp_suf;
		
	}
	
	if (cf)
		return 1;
	if (cs)
		return -1;
	
	return 0;
}

AtomVersion* version_dup(AtomVersion* ver) {
	if (!ver)
		return NULL;
	
	AtomVersion* out = NULL;
	AtomVersion* last = NULL;
	AtomVersion* temp = NULL;
	
	AtomVersion* curr;
	
	for (curr = ver; curr; curr = curr->next) {
		temp = malloc(sizeof(AtomVersion));
		if (curr->full_version)
			temp->full_version = strdup(curr->full_version);
		else
			temp->full_version = NULL;
		temp->prefix = curr->prefix;
		temp->v = strdup(curr->v);
		
		if (!out)
			out = temp;
		else
			last->next = temp;
		last = temp;
	}
	last->next = NULL;
	
	return out;
}

AtomFlag* atomflag_dup(AtomFlag* use) {
	if (!use)
		return NULL;
	
	AtomFlag* out = NULL;
	AtomFlag* last = NULL;
	AtomFlag* temp = NULL;
	
	AtomFlag* curr;
	
	for (curr = use; curr; curr = curr->next) {
		temp = malloc(sizeof(AtomFlag));
		temp->name = strdup(curr->name);
		temp->def = curr->def;
		temp->option = curr->option;
		
		if (!out)
			out = temp;
		else
			last->next = temp;
		last = temp;
	}
	last->next = NULL;
	
	return out;
}

P_Atom* atom_dup(P_Atom* atom) {
	P_Atom* out = malloc(sizeof(P_Atom));
	out->version = version_dup(atom->version);
	out->useflags = atomflag_dup(atom->useflags);
	
	if (atom->slot)
		out->slot = strdup(atom->slot);
	else
		out->slot = NULL;
	
	if (atom->sub_slot)
		out->sub_slot = strdup(atom->sub_slot);
	else
		out->sub_slot = NULL;
	
	out->key = strdup(atom->key);
	out->name = strdup(atom->name);
	out->category = strdup(atom->category);
	out->repository = strdup(atom->repository);
	out->blocks = atom->blocks;
	out->revision = atom->revision;
	out->range = atom->range;
	out->sub_opts = atom->sub_opts;
	out->repo_selected = atom->repo_selected;
	out->parent = atom->parent;
	
	return out;
}

void prv_get_prefix(char* out, atom_version_t range) {
	out[0] = 0;
	out[1] = 0;
	out[2] = 0;
	
	if (range == ATOM_VERSION_ALL)
		;
	else if (range == ATOM_VERSION_REV)
		out[0] = '~';
	else if (range == ATOM_VERSION_E)
		out[0] = '=';
	else if (range & ATOM_VERSION_G)
		out[0] = '>';
	else if (range & ATOM_VERSION_L)
		out[0] = '<';
	
	if (range == ATOM_VERSION_GE || range == ATOM_VERSION_LE)
		out[1] = '=';
}

char* atom_get_str(P_Atom* atom) {
	char* out;
	char prefix[3];
	prv_get_prefix(prefix, atom->range);
	
	char flags[1024];
	flags[0] = 0;
	for (AtomFlag* af = atom->useflags; af; af = af->next) {
		/*
		ATOM_USE_DISABLE, //!< atom[-bar]
		ATOM_USE_ENABLE, //!< atom[bar]
		ATOM_USE_ENABLE_IF_ON, //!< atom[bar?]
		ATOM_USE_DISABLE_IF_OFF, //!< atom[!bar?]
		ATOM_USE_EQUAL, //!< atom[bar=]
		ATOM_USE_OPPOSITE //!< atom[!bar=]
		 */
		
		if (af->option == ATOM_USE_DISABLE)
			strcat(flags, "-");
		else if (af->option == ATOM_USE_DISABLE_IF_OFF)
			strcat(flags, "!");
		
		strcat(flags, af->name);
		if (af->def == ATOM_DEFAULT_ON)
			strcat(flags, "(+)");
		else if (af->def == ATOM_DEFAULT_OFF)
			strcat(flags, "(-)");
		
		if (af->option == ATOM_USE_ENABLE_IF_ON || af->option == ATOM_USE_DISABLE_IF_OFF)
			strcat(flags, "?");
		else if (af->option == ATOM_USE_EQUAL || af->option == ATOM_USE_OPPOSITE)
			strcat(flags, "=");
		
		strcat(flags, " ");
	}
	
	char* version_str = strdup("");
	if (atom->range != ATOM_VERSION_ALL)
		asprintf(&version_str, "-%s", atom->version->full_version);
	
	char* atom_with_slot = NULL;
	if (atom->slot != NULL && atom->sub_slot != NULL)
		asprintf(&atom_with_slot, "%s%s:%s/%s", atom->key, version_str, atom->slot, atom->sub_slot);
	else if (atom->slot != NULL)
		asprintf(&atom_with_slot, "%s%s:%s", atom->key, version_str, atom->slot);
	else
		asprintf(&atom_with_slot, "%s%s", atom->key, version_str);
	
	if (!flags[0])
		asprintf(&out, "%s%s", prefix, atom_with_slot);
	else
		asprintf(&out, "%s%s [ %s]", prefix, atom_with_slot, flags);
	
	free(version_str);
	free(atom_with_slot);

	return out;
}