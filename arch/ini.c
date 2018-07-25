/*
 * ini.c
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


#include "ini.h"
#include "string.h"

aabs_ini_t* aabs_ini_parse(char* path) {
	return conf_new(path);
}

aabs_svec_t* aabs_ini_getsections(aabs_ini_t* ini) {
	aabs_svec_t* out = string_vector_new();
	
	int i;
	for (i = 0; i != ini->sections->n; i++)
		//string_vector_add(out, (*(aabs_inisec_t**) string_vector_get(ini->sections, i)));
	
	return out;
}

char* aabs_ini_getvariable(aabs_ini_t* ini, char* section, char* varname) {
	return conf_get(ini, section, varname);
}

char* aabs_ini_getsecvariable(aabs_inisec_t* sec, char* varname) {
	/*int i;
	for (i = 0; i != sec->variables->n; i++) {
		aabs_inivar_t* curr_var = *(aabs_inivar_t**) vector_get(sec->variables, i);
		if (strcmp(curr_var->identifier, varname) == 0)
			return curr_var->value;
	}
	*/
	return NULL;
}
