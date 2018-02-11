/*
 * deps.h
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

#ifndef AUTOGENTOO_DEPENDENCY_H
#define AUTOGENTOO_DEPENDENCY_H

typedef struct __aabs_depend_t aabs_depend_t;

typedef enum {
	AABS_DEP_MOD_ANY = 1,
	AABS_DEP_MOD_EQ,
	AABS_DEP_MOD_GE,
	AABS_DEP_MOD_LE,
	AABS_DEP_MOD_GT,
	AABS_DEP_MOD_LT
} aabs_depmod_t;

struct __aabs_depend_t {
	char* name;
	char* version;
	char* desc;
	unsigned long name_hash;
	aabs_depmod_t mod;
};

aabs_depend_t* aabs_dep_from_str(char* dep_str);

#endif //AUTOGENTOO_DEPENDENCY_H
