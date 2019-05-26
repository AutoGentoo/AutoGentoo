//
// Created by atuser on 5/5/19.
//

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include "use.h"
#include "package.h"
#include "directory.h"
#include "portage.h"
#include <stdlib.h>
#include <share.h>
#include <string.h>
#include<ctype.h>

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
		else if (*name == '-') {
			status = USE_DISABLE;
			name++;
		} else
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

AtomFlag* dependency_useflag(Ebuild* resolved, AtomFlag* new_flags, AtomFlag* old_flags) {
	AtomFlag* parent = atomflag_dup(old_flags);
	AtomFlag* start = parent;
	
	for (AtomFlag* to_check = new_flags; to_check; to_check = to_check->next) {
		AtomFlag* current = NULL;
		for (AtomFlag* subcheck = start; subcheck; subcheck = subcheck->next) {
			if (strcmp(to_check->name, subcheck->name) != 0)
				continue;
			current = subcheck;
			break;
		}
		if (!current) {
			// Use flag from two not found in one
			// Add this flag
			use_select_t ebuild_setting = package_check_use(resolved, to_check->name);
			if (ebuild_setting == -1) {
				if (to_check->def == ATOM_DEFAULT_OFF)
					ebuild_setting = USE_DISABLE;
				else if (to_check->def == ATOM_DEFAULT_ON)
					ebuild_setting = USE_ENABLE;
				else
					portage_die("Ebuild %s-%s does not have %s flag set and no default was defined",
							resolved->parent->key, resolved->version->full_version,
							to_check->name);
			}
			else {
				/* Doesn't make a difference */
				if (ebuild_setting == USE_DISABLE && to_check->option == ATOM_USE_DISABLE)
					continue;
				else if (ebuild_setting == USE_ENABLE && to_check->option == ATOM_USE_ENABLE)
					continue;
			}
			
			AtomFlag* toadd = malloc(sizeof(AtomFlag));
			toadd->name = strdup(to_check->name);
			toadd->next = parent;
			toadd->option = to_check->option;
			toadd->def = to_check->def;
			parent = toadd;
			continue;
		}
		
		// Use flag from two was found in one, compare them
		if (current->option > ATOM_USE_ENABLE)
			portage_die("P_ATOM option must be simplified before added to dependency tree");
		
		if (current->option != to_check->option) {
		
		}
	}
}

char* strlwr(char* str) {
	unsigned char* p = (unsigned char*)str;
	while (*p) {
		*p = tolower(*p);
		p++;
	}
	
	return str;
}

PackageUse* useflag_parse(FILE* fp, PackageUse** last) {
	char* line = NULL;
	size_t n = 0;
	size_t read_size = 0;
	
	PackageUse* out = NULL;
	PackageUse* temp = NULL;
	
	while ((read_size = getline(&line, &n, fp)) != -1) {
		if (line[0] == '#' || line[0] == '\n' || line[0] == 0)
			continue;
		line[read_size - 1] = 0;
		char* atom_splt = strchr(line, ' ');
		*atom_splt = 0;
		char* atom = strdup(line);
		
		temp = malloc(sizeof(PackageUse));
		temp->atom = atom_parse(atom);
		temp->flags = NULL;
		free(atom);
		
		for (char* curr_flag = strtok(atom_splt + 1, " "); curr_flag; curr_flag = strtok(NULL, " ")) {
			size_t len = strlen(curr_flag);
			if (curr_flag[len - 1] == ':') { // USE_EXPAND
				char* use_expand_flag = strtok(NULL, " ");
				if (!use_expand_flag)
					portage_die("Expected USE_EXPAND flag for %s (package.use)", curr_flag);
				curr_flag[len - 1] = '_';
				curr_flag = strlwr(curr_flag);
				
				asprintf(&curr_flag, "%s%s", curr_flag, use_expand_flag);
				
				UseFlag* new_flag = useflag_new(curr_flag, USE_ENABLE);
				new_flag->next = temp->flags;
				temp->flags = new_flag;
				
				free(curr_flag);
				continue;
			}
			
			UseFlag* new_flag = useflag_new(curr_flag, USE_ENABLE);
			new_flag->next = temp->flags;
			temp->flags = new_flag;
		}
		
		if (!out)
			*last = temp;
		
		temp->next = out;
		out = temp;
	}
	free(line);
	
	return out;
}

void emerge_parse_useflags(Emerge* emerge) {
	char path[256];
	sprintf(path, "%setc/portage/package.use", emerge->root);
	
	FPNode* files = open_directory(path);
	FPNode* old;
	
	PackageUse* parsed = NULL;
	PackageUse* temp = NULL;
	
	for (FPNode* current = files; current;) {
		if (current->type == FP_NODE_DIR) {
			old = current;
			free(old->filename);
			free(old->parent_dir);
			free(old->path);
			current = current->next;
			free(old);
			continue;
		}
		
		FILE* fp = fopen(current->path, "r");
		if (!fp) {
			plog_error("Failed to open %s", current->path);
			fpnode_free(current);
			return;
		}
		
		PackageUse* last = NULL;
		temp = useflag_parse(fp, &last);
		
		last->next = parsed;
		parsed = temp;
		fclose(fp);
		
		old = current;
		free(old->filename);
		free(old->parent_dir);
		free(old->path);
		current = current->next;
		free(old);
	}
	
	PackageUse* next;
	PackageUse* current = parsed;
	PackageUse* new;
	while (current) {
		next = current->next;
		
		for (Repository* repo = emerge->repo; repo; repo = repo->next) {
			if (current->atom->repo_selected == ATOM_REPO_DEFINED
			    && strcmp(current->atom->repository, repo->name) != 0)
				continue;
			
			Package* target = map_get(repo->packages, current->atom->key);
			
			if (!target) {
				plog_warn("Package %s not found (package.accept_keywords)", current->atom->key);
				continue;
			}
			
			new = malloc(sizeof(PackageUse));
			new->next = target->useflags;
			new->atom = atom_dup(current->atom);
			new->flags = NULL;
			
			for (UseFlag* flag = current->flags; flag; flag = flag->next) {
				UseFlag* dupped_flag = malloc(sizeof(UseFlag));
				dupped_flag->name = strdup(flag->name);
				dupped_flag->status = flag->status;
				dupped_flag->next = new->flags;
				new->flags = dupped_flag;
			}
			
			new->next = target->useflags;
			target->useflags = new;
		}
		
		useflag_free(current->flags);
		atom_free(current->atom);
		free(current);
		current = next;
	}
}