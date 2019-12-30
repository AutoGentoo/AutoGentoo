//
// Created by atuser on 10/12/19.
//

#define _GNU_SOURCE

#include "profile.h"
#include "use.h"
#include "directory.h"
#include "portage_log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "language/share.h"
#include <unistd.h>

void profile_make_defaults(Profile* update, FILE* fp, int arg) {
	mc_parse(fp, update->make_conf);
}

void profile_packages(Profile* update, FILE* fp, int arg) {
	char* line = NULL;
	size_t line_size;
	
	while (getline(&line, &line_size, fp) > 0) {
		if (line[0] == '#')
			continue;
		*(strchr(line, '\n')) = 0;
		if (strlen(line) == 0)
			continue;
		
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

void profile_packages_build(Profile* update, FILE* fp, int arg) {
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
	
	
	while (getline(&line, &line_size, fp) > 0) {
		if (*line == '#' || *line == '\n')
			continue;
		line[line_size - 1] = 0; // Remove newline
		
		PackageMask* new_mask = malloc(sizeof(PackageMask));
		new_mask->next = NULL;
		new_mask->mask = default_status;
		
		if (line[0] == '-') {
			new_mask->mask = !default_status;
			new_mask->atom = atom_parse(line + 1);
		}
		else
			new_mask->atom = atom_parse(line);
		
		vector_add(update->package_mask, new_mask);
	}
	
	free(line);
}

void profile_package_mask(Profile* update, FILE* fp, int mask) {
	if (mask)
		profile_package_mask_meta(update, fp, PACKAGE_MASK);
	else
		profile_package_mask_meta(update, fp, PACKAGE_UNMASK);
}

void profile_package_keywords(Profile* update, FILE* fp, int arg) {
	accept_keyword_parse(fp, update->package_accept_keywords);
}

void profile_package_use(Profile* update, FILE* fp, int arg) {
	keyword_t lowest_keyword = KEYWORD_UNSTABLE;
	use_priority_t prior = PRIORITY_NORMAL;
	if (arg & 1)
		lowest_keyword = KEYWORD_STABLE;
	if (arg & 2)
		prior = PRIORITY_FORCE;
	
	useflag_parse(fp, update->package_use, lowest_keyword, prior);
}

void profile_open(Profile* profile, int profile_dir, char* filename, void (*func)(Profile*, FILE*, int), int arg) {
	int fd = openat(profile_dir, filename, O_RDONLY);
	if (fd < 0)
		return; // Profile attribute not included
	
	FILE* fp = fdopen(fd, "r");
	func(profile, fp, arg);
	fclose(fp);
}

void profile_parse(Profile* update, char* current_path, char* path) {
	/* Parse the parents first */
	char* unresolved = NULL;
	asprintf(&unresolved, "%s/%s", current_path, path);
	
	char* fullpath = realpath(unresolved, NULL);
	free(unresolved);
	
	char* parent_filename = NULL;
	asprintf(&parent_filename, "%s/parent", fullpath);
	FILE* fp_parent = fopen(parent_filename, "r");
	
	char* line = NULL;
	size_t line_size;
	
	if (fp_parent) {
		/* This profile has a parent */
		StringVector* parent_profiles = string_vector_new();
		
		while (getline(&line, &line_size, fp_parent) > 0) {
			*strchr(line, '\n') = 0; /* Remove the newline */
			string_vector_add(parent_profiles, line);
		}
		
		free(line);
		fclose(fp_parent);
		
		for (int i = 0; i < parent_profiles->n; i++)
			profile_parse(update, fullpath, string_vector_get(parent_profiles, i));
		
		string_vector_free(parent_profiles);
	}
	
	int prof_dir = open(fullpath, O_RDONLY);
	if (prof_dir < 0)
		portage_die ("Failed to read profile at %s", fullpath);
	
	profile_open(update, prof_dir, "make.defaults", profile_make_defaults, 0);
	
	profile_open(update, prof_dir, "packages", profile_packages, 0);
	profile_open(update, prof_dir, "packages.build", profile_packages_build, 0);
	
	profile_open(update, prof_dir, "package.mask", profile_package_mask, 1);
	profile_open(update, prof_dir, "package.unmask", profile_package_mask, 0);
	
	profile_open(update, prof_dir, "package.keywords", profile_package_keywords, 0); // DEPRACATED
	profile_open(update, prof_dir, "package.accept_keywords", profile_package_keywords, 0);
	
	enum {
		PACKAGE_ARG_NONE = 0,
		PACKAGE_ARG_STABLE = 1 << 0,
		PACKAGE_ARG_FORCE = 1 << 1
	};
	profile_open(update, prof_dir, "package.use", profile_package_use, PACKAGE_ARG_NONE);
	profile_open(update, prof_dir, "package.use.stable", profile_package_use, PACKAGE_ARG_STABLE);
	profile_open(update, prof_dir, "package.use.force", profile_package_use, PACKAGE_ARG_FORCE);
	profile_open(update, prof_dir, "package.use.stable.force", profile_package_use, PACKAGE_ARG_STABLE | PACKAGE_ARG_FORCE);
	
	close(prof_dir);
	free(fullpath);
}

Profile* profile_new() {
	Profile* out = malloc(sizeof(Profile));
	out->make_conf = map_new(256, 0.8);
	out->packages = vector_new(VECTOR_REMOVE | VECTOR_ORDERED);
	out->profile_packages = vector_new(VECTOR_REMOVE | VECTOR_ORDERED);
	out->package_build = vector_new(VECTOR_REMOVE | VECTOR_ORDERED);
	out->package_accept_keywords = vector_new(VECTOR_REMOVE | VECTOR_ORDERED);
	out->package_mask = vector_new(VECTOR_REMOVE | VECTOR_ORDERED);
	out->package_use = vector_new(VECTOR_REMOVE | VECTOR_ORDERED);
	out->use = map_new(512, 0.8);
	
	/* Not implemented yet */
	out->package_bashrc = NULL;
	out->package_provided = NULL;
}

void profile_init(Profile* prof) {
	char profile_realpath[1024];
	size_t profile_len = 0;
	if ((profile_len = readlink("/etc/portage/make.profile", profile_realpath, sizeof(profile_realpath)-1)) == -1) {
		portage_die("Failed to read make.profile link");
	}
	
	profile_realpath[profile_len] = 0;
	profile_parse(prof, "", profile_realpath);
}

void profile_implicit_use(Profile* update) {
	/*
	 * IUSE_IMPLICIT="prefix prefix-guest"
USE_EXPAND_UNPREFIXED="ARCH"
USE_EXPAND_IMPLICIT="ARCH ELIBC KERNEL USERLAND"
USE_EXPAND_VALUES_ARCH="alpha amd64 amd64-fbsd amd64-linux arm arm-linux arm64 hppa hppa-hpux ia64 ia64-hpux ia64-linux m68k m68k-mint mips nios2 ppc ppc64 ppc64-linux ppc-aix ppc-macos ppc-openbsd riscv s390 sh sparc sparc64-freebsd sparc64-solaris sparc-fbsd sparc-solaris x64-cygwin x64-freebsd x64-macos x64-openbsd x64-solaris x86 x86-cygwin x86-fbsd x86-freebsd x86-interix x86-linux x86-macos x86-netbsd x86-openbsd x86-solaris x86-winnt"
USE_EXPAND_VALUES_ELIBC="AIX Cygwin Darwin DragonFly FreeBSD glibc HPUX Interix mintlib musl NetBSD OpenBSD SunOS uclibc Winnt"
USE_EXPAND_VALUES_KERNEL="AIX Darwin FreeBSD freemint HPUX linux NetBSD OpenBSD SunOS Winnt"
USE_EXPAND_VALUES_USERLAND="BSD GNU"
	 
	 */
	
	
}