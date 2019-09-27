//
// Created by atuser on 9/24/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <autogentoo/hacksaw/set.h>
#include <autogentoo/hacksaw/log.h>

Set* set_new(element_cmp cmp_func) {
	Set* out = malloc(sizeof(Set));
	out->parent = vector_new(sizeof(void*), VECTOR_REMOVE | VECTOR_UNORDERED);
	out->cmp_func = cmp_func;
	
	return out;
}

void* set_get(Set* s, int i) {
	return *(void**)vector_get(s->parent, i);
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

void set_add(Set* set, void* element) {
	int index = set_inside(set, element);
	
	if (index == -1)
		vector_add(set->parent, &element);
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
