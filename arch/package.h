/*
 * package.h
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

#ifndef AUTOGENTOO_ABS_PACKAGE_H
#define AUTOGENTOO_ABS_PACKAGE_H

#include "aabs.h"

typedef struct __aabs_pkg_t aabs_pkg_t;

typedef enum {
	AABS_PKG_REASON_WORLD = 0, //!< Pull in by world (defined in DBPATH/world)
	AABS_PKG_REASON_DEPEND = 1
} aabs_pkgreason_t;

typedef enum {
	AABS_PKG_VALIDATION_UNKNOWN = 0,
	AABS_PKG_VALIDATION_NONE = (1 << 0),
	AABS_PKG_VALIDATION_MD5SUM = (1 << 1),
	AABS_PKG_VALIDATION_SHA256SUM = (1 << 2),
	AABS_PKG_VALIDATION_SIGNATURE = (1 << 3)
} aabs_pkgvalidation_t;

struct __aabs_pkg_t {
	unsigned long name_hash;
	char* filename;
	char* base;
	char* name;
	char* version;
	char* desc;
	char* url;
	char* packager;
	char* md5sum;
	char* sha256sum;
	char* base64_sig;
	char* arch;
	
	/* The binary archive */
	struct archive* mtree;
	
	/* This is simply a list provided by the db */
	aabs_svec_t* files;
	aabs_vec_t* backup;
	
	aabs_svec_t* licenses;
	aabs_svec_t* replaces;
	aabs_svec_t* groups;
	aabs_vec_t* depends;
	aabs_vec_t* optdepends;
	aabs_vec_t* conflicts;
	aabs_vec_t* provides;
	
	/* Currently unused */
	aabs_vec_t* makedepends;
	aabs_vec_t* checkdepends;
	
	aabs_time_t builddate;
	aabs_time_t installdate;
	
	off_t size;
	off_t isize;
	off_t download_size;
	
	aabs_pkgreason_t reason;
	aabs_pkgvalidation_t validation;
};

static void parseEVR(char* evr, const char** ep, const char** vp,
					 const char** rp);

static int rpmvercmp(const char* a, const char* b);

int aabs_pkg_vercmp(const char* a, const char* b);

#endif //AUTOGENTOO_ABS_PACKAGE_H
