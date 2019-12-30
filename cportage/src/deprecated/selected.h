//
// Created by atuser on 12/14/19.
//

#ifndef CPORTAGE_SELECTED_H
#define CPORTAGE_SELECTED_H

#include <autogentoo/hacksaw/map.h>
#include "resolve.h"

struct __Selected {
	Map* seleted;
	Map* package_to_slot;
	Vector* head;
};

char* selected_get_key(ResolvedPackage* pkg);
Selected* selected_new();
ResolvedPackage* selected_select(Selected* selected, ResolvedPackage* pkg);
void selected_register_parent(ResolvedPackage* child, SelectedBy* parent_expr);
void selected_unregister_parent(ResolvedPackage* child, ResolvedPackage* parent);

ResolvedPackage* selected_get(Selected* sel, ResolvedPackage* check);
ResolvedPackage* selected_get_atom(Selected* sel, P_Atom* check);

void selected_free(Selected* ptr);

int selected_set_cmp(void* ptr1, void* ptr2);

#endif //CPORTAGE_SELECTED_H
