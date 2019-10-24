//
// Created by atuser on 10/12/19.
//

#define _GNU_SOURCE

#include "profile.h"
#include "use.h"
#include "directory.h"
#include "portage_log.h"
#include <linux/limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <share.h>

void profile_make_defaults(Profile* update, FILE* fp) {
	mc_parse(fp, update->make_defaults);
}

void profile_packages(Profile* update, FILE* fp) {
	char* line = NULL;
	size_t line_size;
	
	while (getline(&line, &line_size, fp) > 0) {
		if (line[0] == '#')
			continue;
		line[line_size - 1] = 0; // Remove newline
		
		if (line[0] == '*') { // System set
			P_Atom* current_atom = atom_parse(line + 1);
			if (current_atom)
				vector_add(update->packages, current_atom);
		}
		else { // Profile Set
			P_Atom* current_atom = atom_parse(line);
			if (current_atom)
				vector_add(update->profile_packages, current_atom);
		}
	}
	
	free(line);
}

void profile_packages_build(Profile* update, FILE* fp) {
	char* line = NULL;
	size_t line_size;
	
	while (getline(&line, &line_size, fp) > 0) {
		if (*line == '#' || *line == '\n')
			continue;
		line[line_size - 1] = 0; // Remove newline
		
		P_Atom* current_atom = atom_parse(line + 1);
		if (current_atom)
			vector_add(update->package_build, current_atom);
	}
	
	free(line);
}

void profile_package_mask_meta(Profile* update, FILE* fp, mask_t default_status) {
	char* line = NULL;
	size_t line_size;
	
	PackageMask** next_mask = update->package_mask ? update->package_mask->last : &update->package_mask;
	
	while (getline(&line, &line_size, fp) > 0) {
		if (*line == '#' || *line == '\n')
			continue;
		line[line_size - 1] = 0; // Remove newline
		
		*next_mask = malloc(sizeof(PackageMask));
		(*next_mask)->next = NULL;
		(*next_mask)->mask = default_status;
		
		if (line[0] == '-') {
			(*next_mask)->mask = !default_status;
			(*next_mask)->atom = atom_parse(line + 1);
		}
		else
			(*next_mask)->atom = atom_parse(line);
		
		update->package_mask->last = &(*next_mask)->next;
		next_mask = update->package_mask->last;
	}
	
	free(line);
}

void profile_package_mask(Profile* update, FILE* fp) {
	profile_package_mask_meta(update, fp, PACKAGE_MASK);
}

void profile_package_unmask(Profile* update, FILE* fp) {
	profile_package_mask_meta(update, fp, PACKAGE_UNMASK);
}

void profile_package_keywords(Profile* update, FILE* fp) {
	accept_keyword_parse(fp, update->package_accept_keywords);
}

void profile_open(Profile* profile, int profile_dir, char* filename, void (*func)(Profile*, FILE*)) {
	int fd = openat(profile_dir, filename, O_RDONLY);
	if (fd < 0)
		return; // Profile attribute not included
	
	FILE* fp = fdopen(fd, "r");
	func(profile, fp);
	fclose(fp);
}

void profile_parse(Profile* update, char* current_path, char* path) {
	/* Parse the parents first */
	char* unresolved = NULL;
	asprintf(&unresolved, "%s/%s", current_path, path);
	
	char fullpath[PATH_MAX];
	realpath(unresolved, fullpath);
	free(unresolved);
	
	char* parent_filename = NULL;
	asprintf(&parent_filename, "%s/parent", fullpath);
	FILE* fp_parent = fopen(parent_filename, "r");
	free(fp_parent);
	
	char* line = NULL;
	size_t line_size;
	
	if (fp_parent) {
		/* This profile has a parent */
		while (getline(&line, &line_size, fp_parent) > 0) {
			line[line_size - 1] = 0; /* Remove the newline */
			profile_parse(update, fullpath, line);
		}
		
		free(line);
		fclose(fp_parent);
	}
	
	int prof_dir = open(fullpath, O_RDONLY);
	if (prof_dir < 0)
		portage_die ("Failed to read profile at %s", fullpath);
	
	profile_open(update, prof_dir, "make.defaults", profile_make_defaults);
	
	profile_open(update, prof_dir, "packages", profile_packages);
	profile_open(update, prof_dir, "packages.build", profile_packages_build);
	
	profile_open(update, prof_dir, "package.mask", profile_package_mask);
	profile_open(update, prof_dir, "package.unmask", profile_package_unmask);
	
	profile_open(update, prof_dir, "package.keywords", profile_package_keywords); // DEPRACATED
	profile_open(update, prof_dir, "package.accept_keywords", profile_package_keywords);
	
	profile_open(update, prof_dir, "package.use", profile_package_keywords);
}