/*
 * write.c
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

#include "write.h"
#include "aabs.h"

void write_dest_section (FILE* fp, char* name, char* value) {
    fprintf (fp, "%%%s%%\n%s\n\n", name, value);
}

void write_dest_vector (FILE* fp, char* name, aabs_svec_t* vec) {
    fprintf (fp, "%%%s%%", name);
    
    int i;
    for (i = 0; i != vec->n; i++) {
        fprintf (fp, "%s\n", string_vector_get(vec, i));
    }
    
    fwrite ("\n", 1, 1, fp);
}
