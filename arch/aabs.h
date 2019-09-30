/*
 * aabs.h
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

#ifndef AUTOGENTOO_ABS_AABS_H
#define AUTOGENTOO_ABS_AABS_H

#include <sys/types.h>
#include <autogentoo/hacksaw/hacksaw.h>

typedef Vector aabs_vec_t;
typedef StringVector aabs_svec_t;
typedef SmallMap aabs_smap_t;
typedef Map aabs_map_t;
typedef LinkedNode aabs_node_t;

typedef struct __aabs_file_t aabs_file_t;
typedef struct __aabs_filelist_t aabs_filelist_t;
typedef struct __aabs_main aabs_main;

typedef int64_t aabs_int64_t;
typedef aabs_int64_t aabs_time_t;


typedef enum {
	AABS_ERR_MEMORY = 1,
	AABS_ERR_NOT_A_FILE,
	AABS_ERR_NOT_A_DIRECTORY,
	AABS_ERR_DISK_SPACE,
	
	/* Function calls */
			AABS_ERR_NULL_PTR,
	
	/* Database */
			AABS_ERR_DB_CREATE,
	AABS_ERR_DB_READ,
	AABS_ERR_DB_NOT_FOUND,
	
	/* Internet */
			AABS_ERR_NET_NOT_FOUND,
	AABS_ERR_NET_SERVER_INVALID,
	
	/* Package */
			AABS_ERR_PKG_NOT_FOUND,
	AABS_ERR_PKG_INVALID,
	AABS_ERR_PKG_INVALID_CHECKSUM,
	AABS_ERR_PKG_INVALID_SIG
} aabs_error_t;

/* The error status of the last failure */
static aabs_error_t aabs_errno;

struct __aabs_file_t {
	char* name;
	off_t size;
	mode_t mode;
};

struct __aabs_filelist_t {
	size_t count;
	aabs_file_t** files;
};

#include "db.h"
#include "package.h"

struct __aabs_main {
	char* rootdir;
	char* dbpath;
	char* cachedir;
	char* logfile;
	char* gpgdir;
	char* hookdir;
	
	char* xfercommand;
	aabs_svec_t* holdpkg;
	char* architecture;
	
	aabs_db_t* local;
	aabs_vec_t* repositories;
};

extern aabs_main* aabs;

void aabs_create(char* config);

aabs_pkg_t* aabs_package_search(char* str);

aabs_pkg_t* aabs_library_search(char* lib);

#endif //AUTOGENTOO_ABS_AABS_H
