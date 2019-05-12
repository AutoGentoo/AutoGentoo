//
// Created by atuser on 4/23/19.
//

#define _GNU_SOURCE

#include "database.h"
#include "portage_log.h"
#include "directory.h"
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

PortageDB* portagedb_read(Emerge* emerge) {
	PortageDB* out = malloc(sizeof(PortageDB));
	
	asprintf(&out->path, "%s/var/db/pkg/", emerge->root);
	int db_dir = open(out->path, O_RDONLY);
	if (db_dir < 0) {
		plog_error("Failed to open database directory: %s", out->path);
		free(out->path);
		free(out);
		return NULL;
	}
	
	FPNode* categories = open_directory_stat(__S_IFDIR, db_dir, NULL);
	
	
	return out;
}