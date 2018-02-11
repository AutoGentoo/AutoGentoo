/*
 * makepkg.h
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


#ifndef AUTOGENTOO_ABS_MAKEPKG_H
#define AUTOGENTOO_ABS_MAKEPKG_H

#include <stdio.h>
#include "aabs.h"
#include "package.h"

typedef struct _aabs_makepkg_t aabs_makepkg_t;

struct _aabs_makepkg_t {
	aabs_pkg_t* target;
	
	char* download_uri;
	aabs_svec_t* checksums;
	
	/* build environment variables */
	char* CHOST;
	char* CFLAGS;
	char* CXXFLAGS;
	
	/* PKGBUILD variables */
	char* PKGDEST;
	char* SRCDEST;
	char* SRCPKGDEST;
	char* LOGDEST;
	char* PACKAGER;
	char* BUILDDIR;
	char* CARCH;
	char* PKGEXT;
	char* GNUPGHOME;
	char* GPGKEY;
	
	struct archive* pkg;
	
	/* Package struct so we can memcpy */
	
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

aabs_makepkg_t* aabs_makepkg_create(char* PKGBUILD);

int aabs_makepkg_prepare(aabs_makepkg_t* target);

int aabs_makepkg_build(aabs_makepkg_t* target);

aabs_pkg_t* aabs_makepkg_handoff(aabs_makepkg_t* target);

#endif