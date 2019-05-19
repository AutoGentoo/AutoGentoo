//
// Created by atuser on 5/5/19.
//

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include "use.h"
#include "package.h"
#include <stdlib.h>

use_select_t package_check_use(Ebuild* ebuild, char* useflag) {
	UseFlag* current;
	for (current = ebuild->use; current; current = current->next) {
		if (strcmp(current->name, useflag) == 0)
			return current->status;
	}
	
	return -1;
}

RequiredUse* use_build_required_use(char* target, use_select_t option) {
	RequiredUse* out = malloc(sizeof(RequiredUse));
	
	if (target)
		out->target = strdup(target);
	else
		out->target = NULL;
	out->option = option;
	out->depend = NULL;
	out->next = NULL;
	
	return out;
}

static char* use_select_prefix[] = {
		"", //None
		"!",
		"",
		"||",
		"^^",
		"??"
};

static char* use_select_suffix[] = {
		"", //None
		"?",
		"?",
		"",
		"",
		""
};

char* use_expr_str(RequiredUse* expr) {
	char* out;
	if (expr->depend) {
		char* depend_str = use_expr_str(expr->depend);
		asprintf(&out, "%s%s%s ( %s )",
		         use_select_prefix[expr->option],
		         expr->target,
		         use_select_suffix[expr->option],
		         depend_str
		);
		free(depend_str);
	}
	else
		asprintf(&out, "%s%s", use_select_prefix[expr->option], expr->target);
	return out;
}

int use_check_expr(Ebuild* ebuild, RequiredUse* expr) {
	if (expr->option == USE_ENABLE || expr->option == USE_DISABLE) {
		if (expr->depend) {
			if (package_check_use(ebuild, expr->target) != expr->option)
				return use_check_expr(ebuild, expr->depend);
		} else {
			if (expr->option == package_check_use(ebuild, expr->target))
				return 1;
			return 0;
		}
	}
	
	int num_true = 0;
	for (RequiredUse* c = expr->depend; c; c = c->next) {
		if (use_check_expr(ebuild, c))
			num_true++;
	}
	
	if (expr->option == USE_LEAST_ONE)
		return num_true >= 1;
	if (expr->option == USE_MOST_ONE)
		return num_true <= 1;
	if (expr->option == USE_EXACT_ONE)
		return num_true == 1;
	
	return 0;
}

int ebuild_check_required_use(Ebuild* ebuild) {
	if (!ebuild->required_use)
		return 1;
	for (RequiredUse* expr = ebuild->required_use; expr; expr = expr->next) {
		if (use_check_expr(ebuild, expr) == 0) {
			char* expr_fail = use_expr_str(expr);
			plog_warn("Required use not met");
			plog_warn(expr_fail);
			free(expr_fail);
			return 0;
		}
	}
	
	return 1;
}

UseFlag* useflag_new(char* name, use_select_t status) {
	UseFlag* out = malloc(sizeof(UseFlag));
	if (status == USE_NONE) {
		if (*name == '+') {
			status = USE_ENABLE;
			name++;
		}
		else
			status = USE_DISABLE;
	}
	
	out->next = NULL;
	out->name = strdup(name);
	out->status = status;
	
	return out;
}

void useflag_free(UseFlag* ptr) {
	UseFlag* next = NULL;
	UseFlag* curr = ptr;
	while (curr) {
		next = curr->next;
		free(curr->name);
		free(curr);
		curr = next;
	}
}

void requireduse_free(RequiredUse* ptr) {
	if (!ptr)
		return;
	
	RequiredUse* next = NULL;
	RequiredUse* temp = ptr;
	while (temp) {
		next = temp->next;
		if (ptr->target)
			free(ptr->target);
		free(ptr);
		temp = next;
	}
	
	temp = ptr->depend;
	while (temp) {
		next = temp->next;
		if (ptr->target)
			free(ptr->target);
		free(ptr);
		temp = next;
	}
	
	if (ptr->target)
		free(ptr->target);
	free(ptr);
}
