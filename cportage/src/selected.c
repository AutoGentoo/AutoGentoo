//
// Created by atuser on 12/14/19.
//

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include "selected.h"
#include "package.h"
#include <autogentoo/hacksaw/set.h>
#include <string.h>

char* selected_get_key(ResolvedPackage* pkg) {
	char* key = NULL;
	ResolvedEbuild* c_ebuild = pkg->current;
	if (!c_ebuild)
		c_ebuild = pkg->ebuilds;
	
	asprintf(&key, "%s:%s", c_ebuild->ebuild->parent->key, c_ebuild->ebuild->slot);
	return key;
}

Selected* selected_new() {
	Selected* out = malloc(sizeof(Selected));
	
	out->head = vector_new(VECTOR_ORDERED | VECTOR_REMOVE);
	out->seleted = map_new(512, 0.8);
	out->package_to_slot = map_new(512, 0.8);
	
	return out;
}

ResolvedPackage* selected_select(Selected* selected, ResolvedPackage* pkg) {
	char* key = selected_get_key(pkg);
	Set* old_set = map_get(selected->package_to_slot, pkg->current->ebuild->parent->key);
	if (!old_set) {
		old_set = set_new((int(*)(void*, void*))strcmp);
		map_insert(selected->package_to_slot, pkg->current->ebuild->parent->key, old_set);
	}
	
	set_add(old_set, pkg->current->ebuild->slot);
	ResolvedPackage* out = map_insert(selected->seleted, key, pkg);
	free(key);
	
	return out;
}

void selected_register_parent(ResolvedPackage* child, SelectedBy* parent_expr) {
	if (!set_add(child->parents, parent_expr)) {
		free(parent_expr);
		return;
	}
}

void selected_unregister_parent(ResolvedPackage* child, ResolvedPackage* parent) {
	SelectedBy* search_for = selected_by_new(parent, NULL);
	SelectedBy* to_free = set_remove(child->parents, search_for);
	free(search_for);
	if (to_free)
		free(search_for);
	
	if (child->parents->parent->n != 0)
		return;
	
	/* No more parents, free this child */
	char* key = selected_get_key(child);
	Map* sel = child->environ->selected->seleted;
	
	resolved_package_free(child);
	
	map_remove(sel, key);
}

ResolvedPackage* selected_get(Selected* sel, ResolvedPackage* check) {
	char* key = selected_get_key(check);
	ResolvedPackage* found = map_get(sel->seleted, key);
	free(key);
	if (found) {
		plog_info("FOUND SELECTED");
		fflush(stdout);
	}
	return found;
}

ResolvedPackage* selected_get_atom(Selected* sel, P_Atom* check) {
	if (check->slot) {
		char* key = NULL;
		asprintf(&key, "%s:%s", check->key, check->slot);
		ResolvedPackage* found = map_get(sel->seleted, key);
		free(key);
		
		if (found && ebuild_match_atom(found->current->ebuild, check))
			return found;
		return NULL;
	}
	
	Set* slots = map_get(sel->package_to_slot, check->key);
	if (!slots)
		return NULL;
	
	for (int i = 0; i < slots->parent->n; i++) {
		char* current_slot = set_get(slots, i);
		char* current_key = NULL;
		asprintf(&current_key, "%s:%s", check->key, current_slot);
		ResolvedPackage* found = map_get(sel->seleted, current_key);
		free(current_key);
		
		if (found && (!found->current || ebuild_match_atom(found->current->ebuild, check)))
			return found;
	}
	
	return NULL;
}

void selected_free(Selected* ptr) {
	for (int i = 0; i < ptr->head->n; i++) {
		selected_unregister_parent(vector_get(ptr->head, i), NULL);
	}
	
	vector_free(ptr->head);
	map_free(ptr->seleted, NULL);
	map_free(ptr->package_to_slot, (void (*)(void*))set_free);
	free(ptr);
}

int selected_set_cmp(void* ptr1, void* ptr2) {
	return ((SelectedBy*)ptr1)->parent == ((SelectedBy*)ptr2)->parent;
}