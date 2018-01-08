/*
 * aabs.c
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


#include "stdio.h"
#include "stdlib.h"
#include "aabs.h"
#include "package.h"
#include "ini.h"

aabs_main* aabs = NULL;

void aabs_create (char* config) {
    aabs = calloc (sizeof (aabs_main), 1);
    
    aabs_ini_t* ini_temp = aabs_ini_parse(config);
    
}

aabs_pkg_t* aabs_package_search (char* str);
aabs_pkg_t* aabs_library_search (char* lib);

