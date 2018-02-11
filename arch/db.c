/*
 * db.c
 * 
 * Copyright 2018 Andrei Tumbar <atuser@Kronos>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <archive.h>
#include <archive_entry.h>
#include "db.h"
#include "deps.h"
#include <linux/limits.h>

aabs_db_read_handler_t db_pkg_read_offsets[] = {
		/* Global read handlers (local and sync) */
		{"%NAME%",         offsetof (aabs_pkg_t, name)},
		{"%VERSION%",      offsetof (aabs_pkg_t, version)},
		{"%BASE%",         offsetof (aabs_pkg_t, base)},
		{"%DESC%",         offsetof (aabs_pkg_t, desc)},
		{"%GROUPS%",       offsetof (aabs_pkg_t, groups),       AABS_DB_HANDLE_TYPE_SVEC},
		{"%URL%",          offsetof (aabs_pkg_t, url)},
		{"%LICENSE%",      offsetof (aabs_pkg_t, licenses),     AABS_DB_HANDLE_TYPE_SVEC},
		{"%ARCH%",         offsetof (aabs_pkg_t, arch)},
		{"%BUILDDATE%",    offsetof (aabs_pkg_t,
									 builddate),                AABS_DB_HANDLE_TYPE_STRING, NULL, NULL, _aabs_parsedate},
		{"%INSTALLDATE%",  offsetof (aabs_pkg_t,
									 installdate),              AABS_DB_HANDLE_TYPE_STRING, NULL, NULL, _aabs_parsedate},
		{"%PACKAGER%",     offsetof (aabs_pkg_t, packager)},
		{"%REASON%",       offsetof (aabs_pkg_t, reason),       AABS_DB_HANDLE_TYPE_STRING, atoi},
		{"%VALIDATION%",   offsetof (aabs_pkg_t, validation),   AABS_DB_HANDLE_TYPE_SVEC,   NULL, (int (*)(
				aabs_svec_t*)) aabs_validation_get},
		{"%SIZE%",         offsetof (aabs_pkg_t,
									 isize),                    AABS_DB_HANDLE_TYPE_STRING, NULL, NULL, _aabs_str_to_off_t},
		{"%REPLACES%",     offsetof (aabs_pkg_t, replaces),     AABS_DB_HANDLE_TYPE_DEP},
		{"%DEPENDS%",      offsetof (aabs_pkg_t, depends),      AABS_DB_HANDLE_TYPE_DEP},
		{"%OPTDEPENDS%",   offsetof (aabs_pkg_t, optdepends),   AABS_DB_HANDLE_TYPE_DEP},
		{"%CONFLICTS%",    offsetof (aabs_pkg_t, conflicts),    AABS_DB_HANDLE_TYPE_DEP},
		{"%PROVIDES%",     offsetof (aabs_pkg_t, provides),     AABS_DB_HANDLE_TYPE_DEP},
		
		/* Sync only read handlers */
		{"%FILENAME%",     offsetof (aabs_pkg_t, filename)},
		{"%DESC%",         offsetof (aabs_pkg_t, desc)},
		{"%CSIZE%",        offsetof (aabs_pkg_t,
									 size),                     AABS_DB_HANDLE_TYPE_STRING, NULL, NULL, _aabs_str_to_off_t},
		{"%ISIZE%",        offsetof (aabs_pkg_t,
									 isize),                    AABS_DB_HANDLE_TYPE_STRING, NULL, NULL, _aabs_str_to_off_t},
		{"%ISIZE%",        offsetof (aabs_pkg_t,
									 isize),                    AABS_DB_HANDLE_TYPE_STRING, NULL, NULL, _aabs_str_to_off_t},
		{"%MD5SUM%",       offsetof (aabs_pkg_t, md5sum)},
		{"%SHA256SUM%",    offsetof (aabs_pkg_t, sha256sum)},
		{"%PGPSIG%",       offsetof (aabs_pkg_t, base64_sig)},
		
		/* According to libalpm these are unused  */
		{"%MAKEDEPENDS%",  offsetof (aabs_pkg_t, makedepends),  AABS_DB_HANDLE_TYPE_SKIP_MULTI},
		{"%CHECKDEPENDS%", offsetof (aabs_pkg_t, checkdepends), AABS_DB_HANDLE_TYPE_SKIP_MULTI},
		
		{"%DELTAS%", 0,                                         AABS_DB_HANDLE_TYPE_SKIP_MULTI},
		{"%FILES%",        offsetof (aabs_pkg_t, files),        AABS_DB_HANDLE_TYPE_SVEC},
		
		{NULL,       0,                                         AABS_DB_HANDLE_TYPE_STRING, NULL, NULL}
};

aabs_db_t* aabs_db_new(char* name, aabs_db_type_t type) {
	aabs_db_t* out;
	MALLOC (out, sizeof(aabs_db_t), exit(1));
	out->packages = map_new(sizeof(aabs_pkg_t*), 128);
	out->type = AABS_DB_TYPE_LOCAL;
	
	ASSERT(name != NULL, lerror("db name cannot be NULL");
			FREE(out);
			aabs_errno = AABS_ERR_NULL_PTR;
			return NULL;
	
	);
	
	out->type = type;
	if (type == AABS_DB_TYPE_SYNC)
		out->type = AABS_DB_TYPE_SYNC;
	
	out->name = name;
	
	return out;
}

void aabs_local_db_write(aabs_db_t* db) {
	if (db->type != AABS_DB_TYPE_LOCAL) {
		lerror("can't write a 'sync' db");
		return;
	}
	
	char* db_ar = aabs_db_archive_path(db);
	char* db_path = aabs_db_path(db);
	
	char* archive_cmd;
	asprintf(&archive_cmd, "cd %s && tar -cf %s *", db_path, db_ar);
	
	system(archive_cmd);
	free(db_ar);
	free(db_path);
	free(archive_cmd);
}

void aabs_db_read(aabs_db_t* db) {
	char* db_path = aabs_db_archive_path(db);
	
	db->obj = archive_read_new();
	struct archive_entry* entry;
	int r;
	
	archive_read_support_filter_all(db->obj);
	archive_read_support_format_all(db->obj);
	
	r = archive_read_open_filename(db->obj, db_path, 10240);
	
	if (r != ARCHIVE_OK) {
		aabs_errno = AABS_ERR_DB_READ;
		lerror("Failed to read db archive");
		return;
	}
	
	aabs_pkg_t* curr_pkg = NULL;
	
	while (archive_read_next_header(db->obj, &entry) == ARCHIVE_OK) {
		char* full_path = (char*) archive_entry_pathname(entry);
		if (strcmp(full_path, "ALPM_DB_VERSION") == 0) {
			continue;
		}
		
		char* file_name = strchr(full_path, '/') + 1;
		*(file_name - 1) = '\0'; // Split the paths
		
		/* Once we enter a new directory we are parsing for a different package */
		if (*file_name == '\0') {
			curr_pkg = malloc(sizeof(aabs_pkg_t));
			memset(curr_pkg, 0, sizeof(aabs_pkg_t));
			continue;
		}
		if (strcmp(file_name, "desc") == 0) {
			size_t current_size = (size_t) archive_entry_size(entry);
			char* buffer = malloc(current_size + 4);
			ssize_t size = archive_read_data(db->obj, buffer, current_size);
			
			FILE* fp = fmemopen(buffer, (size_t) size, "r");
			char line[1024];
			
			while (1) {
				if (READ_NEXT () == 0)
					continue;
				
				int handler_index = aabs_db_handler_get(line);
				if (handler_index == -1) {
					fprintf(stderr, "could not recognize header %s for package %s\n", line, full_path);
					continue;
				}
				
				size_t offset = db_pkg_read_offsets[handler_index].offset;
				if (db_pkg_read_offsets[handler_index].type == AABS_DB_HANDLE_TYPE_STRING) {
					READ_NEXT();
					if (db_pkg_read_offsets[handler_index].single_handler != NULL) {
						int temp = (*db_pkg_read_offsets[handler_index].single_handler)(line);
						memcpy(&((char*) curr_pkg)[offset], &temp, sizeof(int));
					} else if (db_pkg_read_offsets[handler_index].single_handler_large != NULL) {
						aabs_int64_t temp = (*db_pkg_read_offsets[handler_index].single_handler_large)(line);
						memcpy(&((char*) curr_pkg)[offset], &temp, sizeof(aabs_int64_t));
					} else {
						char* temp = strdup(line);
						memcpy(&((char*) curr_pkg)[offset], &temp, sizeof(char*));
					}
				} else if (db_pkg_read_offsets[handler_index].type == AABS_DB_HANDLE_TYPE_SVEC) {
					aabs_svec_t* svec_dest = string_vector_new();
					
					do {
						READ_NEXT();
						if (*line == '\0')
							break;
						string_vector_add(svec_dest, line);
					} while (1);
					
					if (db_pkg_read_offsets[handler_index].list_handler != NULL) {
						int temp = (*db_pkg_read_offsets[handler_index].list_handler)(svec_dest);
						memcpy(&((char*) curr_pkg)[offset], &temp, sizeof(int));
					} else {
						memcpy(&((char*) curr_pkg)[offset], &svec_dest, sizeof(void*));
					}
				} else if (db_pkg_read_offsets[handler_index].type == AABS_DB_HANDLE_TYPE_DEP) {
					/* I want it ordered in anticipation of multiple constraints on one dependency problem */
					aabs_vec_t* vec_dest = vector_new(sizeof(aabs_depend_t*), REMOVE | ORDERED);
					
					do {
						READ_NEXT();
						if (*line == '\0')
							break;
						aabs_depend_t* k = aabs_dep_from_str(line);
						vector_add(vec_dest, &k);
					} while (1);
					
					memcpy(&((char*) curr_pkg)[offset], &vec_dest, sizeof(void*));
				} else if (db_pkg_read_offsets[handler_index].type == AABS_DB_HANDLE_TYPE_SKIP_SINGLE) {
					READ_NEXT();
				} else if (db_pkg_read_offsets[handler_index].type == AABS_DB_HANDLE_TYPE_SKIP_MULTI) {
					do {
						READ_NEXT();
						if (*line == '\0')
							break;
					} while (1);
				}
			}
			
			fclose(fp);
			free(buffer);
			
			curr_pkg->name_hash = map_insert(db->packages, curr_pkg->name, &curr_pkg);
		} else if (strcmp(file_name, "files") == 0) {
			size_t current_size = (size_t) archive_entry_size(entry);
			char* buffer = malloc(current_size + 4);
			ssize_t size = archive_read_data(db->obj, buffer, current_size);
			
			FILE* fp = fmemopen(buffer, (size_t) size, "r");
			char line[PATH_MAX];
			
			/* %FILES% */
			READ_NEXT_NO_BREAK();
			
			curr_pkg->files = string_vector_new();
			curr_pkg->backup = string_vector_new();
			
			while (1) {
				if (READ_NEXT () == 0)
					break;
				string_vector_add(curr_pkg->files, line);
			};
			
			/* Just in case we reach end of file */
			READ_NEXT_NO_BREAK();
			
			if (strcmp(line, "%BACKUP%") == 0) {
				while (1) {
					if (READ_NEXT () == 0)
						break;
					string_vector_add(curr_pkg->backup, line);
				};
			}
			
			fclose(fp);
			free(buffer);
		}
	}
	
	free(db_path);
}

char* aabs_db_archive_path(aabs_db_t* db) {
	char* out = NULL;
	MALLOC (out, strlen(db->name) + strlen(DBPATH) + 32, exit(1));
	sprintf(out, DBPATH "%s/%s.db",
			db->type == AABS_DB_TYPE_LOCAL ? "" : "/sync",
			db->name);
	
	return out;
}

char* aabs_db_path(aabs_db_t* db) {
	char* out = NULL;
	MALLOC (out, strlen(DBPATH) + 32, exit(1));
	sprintf(out, DBPATH "%s/", db->type == AABS_DB_TYPE_LOCAL ? "/local" : "/sync");
	
	return out;
}

char* aabs_local_db_pkgpath(aabs_db_t* db,
							aabs_pkg_t* info,
							const char* filename) {
	size_t len;
	char* pkgpath;
	char* dbpath;
	
	dbpath = aabs_db_path(db);
	len = strlen(dbpath) + strlen(info->name) + strlen(info->version) + 3;
	len += filename ? strlen(filename) : 0;
	MALLOC(pkgpath, len, exit(1));
	sprintf(pkgpath, "%s%s-%s/%s", dbpath, info->name, info->version,
			filename ? filename : "");
	free(dbpath);
	return pkgpath;
}

void aabs_local_write_db(aabs_db_t* db, aabs_pkg_t* pkg, aabs_db_read_t opts) {
	FILE* fp = NULL;
	
	char* target_dir = aabs_local_db_pkgpath(db, pkg, NULL);
	
	if (opts & DB_BASE) {
		prv_mkdir(target_dir);
	}
	
	if (opts & DB_DESC) {
	
	}
}

aabs_pkgvalidation_t aabs_validation_get(aabs_svec_t* vec) {
	aabs_pkgvalidation_t out = (aabs_pkgvalidation_t) 0;
	
	int i;
	for (i = 0; i != vec->n; i++) {
		char* data = string_vector_get(vec, i);
		if (strcmp(data, "none") == 0) {
			out |= AABS_PKG_VALIDATION_NONE;
		} else if (strcmp(data, "md5") == 0) {
			out |= AABS_PKG_VALIDATION_MD5SUM;
		} else if (strcmp(data, "sha256") == 0) {
			out |= AABS_PKG_VALIDATION_SHA256SUM;
		} else if (strcmp(data, "pgp") == 0) {
			out |= AABS_PKG_VALIDATION_SIGNATURE;
		} else {
			lerror("unknown validation type for package %s: %s\n",
				   "", (const char*) data);
		}
	}
	
	return out;
}

int aabs_db_handler_get(char* name) {
	int i;
	for (i = 0; db_pkg_read_offsets[i].desc_header != NULL
				&& strcmp(db_pkg_read_offsets[i].desc_header, name) != 0;
		 i++);
	return db_pkg_read_offsets[i].desc_header == NULL ? -1 : i;
}
