//
// Created by atuser on 4/23/19.
//

#include "package.h"
#include "portage_log.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int wildcard_cmp(char* const_str, char* wildcard_str) {
	char* wildcard_ptr = strchr(wildcard_str, '*');
	if (!wildcard_ptr)
		return strcmp(const_str, wildcard_ptr);
	return strncmp (const_str, wildcard_str, wildcard_ptr - wildcard_str);
}

atom_cmp_t ebuild_atom_compare(Ebuild* ebuild, P_Atom* atom) {
	return 0;
}

P_Atom* atom_new(char* cat, char* name) {
	P_Atom* out = malloc(sizeof(P_Atom));
	out->category = strdup(cat);
	out->name = strdup(name);
	out->range = ATOM_VERSION_ALL;
	out->blocks = ATOM_BLOCK_NONE;
	
	return out;
}

AtomNode* atom_version_new(char* version_str) {
	AtomNode* parent = malloc (sizeof(AtomNode));
	AtomNode* current_node = parent;
	
	char* buf = strtok(version_str, "._-");
	while (buf) {
		current_node->v = strdup(buf);
		if ((buf = strtok(NULL, "._-"))) {
			current_node->next = malloc(sizeof(AtomNode));
			current_node = current_node->next;
		}
		else
			current_node->next = NULL;
	}
	
	return parent;
}

void atomnode_free(AtomNode* parent) {
	AtomNode* next;
	for (next = parent->next; parent; parent = next)
		free(parent->v);
}

void atom_free(P_Atom* ptr) {
	atomnode_free(ptr->version);
	atomnode_free(ptr->useflags);
	
	free(ptr->category);
	free(ptr->name);
	free(ptr);
}