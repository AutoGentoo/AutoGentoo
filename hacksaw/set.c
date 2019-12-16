//
// Created by atuser on 9/24/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <autogentoo/hacksaw/set.h>
#include <autogentoo/hacksaw/log.h>

Set* set_new(element_cmp cmp_func) {
	Set* out = malloc(sizeof(Set));
	out->parent = vector_new(VECTOR_REMOVE | VECTOR_UNORDERED);
	out->cmp_func = cmp_func;
	
	return out;
}

void* set_get(Set* s, int i) {
	return vector_get(s->parent, i);
}

int set_inside(Set* s, void* item) {
	for (int i = 0; i < s->parent->n; i++) {
		if (s->cmp_func && s->cmp_func(item, set_get(s, i)) == 0)
			return i;
		else if (!s->cmp_func && item == set_get(s, i))
			return i;
	}
	
	return -1;
}

int set_add(Set* set, void* element) {
	int index = set_inside(set, element);
	
	if (index == -1) {
		vector_add(set->parent, element);
		return 1;
	}

	return 0;
}

void* set_remove(Set* set, void* element) {
	int index = set_inside(set, element);
	if (index == -1)
		return NULL;
	
	return vector_remove(set->parent, index);
}

Set* set_dup(Set* s) {
	Set* out = set_new(s->cmp_func);
	
	for (int i = 0; i < s->parent->n; i++)
		set_add(out, set_get(s, i));
	
	return out;
}

void set_union(Set* s1, Set* s2) {
	if (s1->cmp_func != s2->cmp_func) {
		lerror("Invalid Set* cmp_func for set_union()");
		return;
	}
	
	for (int i = 0; i < s2->parent->n; i++)
		set_add(s1, set_get(s2, i));
}

Set* set_intersect(Set* s1, Set* s2) {
	if (s1->cmp_func != s2->cmp_func) {
		lerror("Invalid Set* cmp_func for set_intersect()");
		return NULL;
	}
	
	Set* out = set_new(s1->cmp_func);
	
	/* Iterate through the bigger one */
	if (s1->parent->n > s2->parent->n) {
		for (int i = 0; i < s1->parent->n; i++) {
			void* element = set_get(s1, i);
			if (set_inside(s2, element) != -1)
				set_add(out, element);
		}
	}
	else {
		for (int i = 0; i < s2->parent->n; i++) {
			void* element = set_get(s2, i);
			if (set_inside(s1, element) != -1)
				set_add(out, element);
		}
	}
	
	return out;
}

Set* set_collapse(Set* to_collapse, void* (*merge_func)(void*, void*)) {
	Set* out_set = set_new(to_collapse->cmp_func);
	Set* has_read = set_new(to_collapse->cmp_func);
	
	for (int i = 0; i < to_collapse->parent->n; i++) {
		void* el_1 = set_get(to_collapse, i);
		if (set_inside(has_read, el_1) != -1)
			continue;
		
		for (int j = i + 1; j < to_collapse->parent->n; j++) {
			void* el_2 = set_get(to_collapse, j);
			
			if (set_inside(has_read, el_2) != -1)
				continue;
			
			void* out_el = merge_func(el_1, el_2);
			if (out_el) {
				set_add(has_read, el_2);
				set_add(out_set, out_el);
			}
		}
	}
	
	set_free(has_read);
	
	return out_set;
}

void set_free(Set* ptr) {
	vector_free(ptr->parent);
	free(ptr);
}
