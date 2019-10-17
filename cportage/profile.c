//
// Created by atuser on 10/12/19.
//

#define _GNU_SOURCE

#include "profile.h"
#include "use.h"
#include "directory.h"
#include "portage_log.h"
#include "globals.h"
#include <linux/limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <share.h>

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
	
	if (fp_parent) {
		/* This profile has a parent */
		char* line = NULL;
		size_t line_size;
		
		while (getline(&line, &line_size, fp_parent) > 0) {
			line[line_size - 1] = 0; /* Remove the newline */
			profile_parse(update, fullpath, line);
		}
		
		free(line);
		fclose(fp_parent);
	}
	
	FPNode* dir = open_directory(fullpath);
	if (!dir)
		portage_die ("Failed to read profile at %s", fullpath);
	
	int dir_fd = open(fullpath, O_RDONLY);
	for (FPNode* current_file = dir; current_file; current_file = current_file->next) {
		if (current_file->type != FP_NODE_FILE)
			continue; /* Skip all subdirectories */
		
		FILE* fp = fopen(current_file->path, "r");
		if (!fp) {
			plog_error("Failed to open profile file: %s", current_file->path);
			continue;
		}
		
		if (strcmp(current_file->filename, "make.defaults") == 0) {
			mc_parse(fp, update->make_defaults);
		}
	}
}