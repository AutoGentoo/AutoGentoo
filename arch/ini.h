/*
 * ini.h
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

#include <autogentoo/hacksaw/tools.h>
#include "aabs.h"

typedef Conf aabs_ini_t;
typedef ConfSection aabs_inisec_t;
typedef ConfVariable aabs_inivar_t;

aabs_ini_t* aabs_ini_parse (char* path);
aabs_svec_t* aabs_ini_getsections (aabs_ini_t* ini);
char* aabs_ini_getvariable(aabs_ini_t* ini, char* section, char* varname);
char* aabs_ini_getsecvariable(aabs_inisec_t* sec, char* varname);
