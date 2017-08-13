/*
 * list.c
 * 
 * Copyright 2017 Unknown <atuser@Hyperion>
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


#include <list.h>

_list * list_init (size_t type) {
    _list *out = malloc (sizeof (_list));
    out->el_size = type;
    out->total = _LIST_INC_SIZE;
    out->used = 0;
    out->el = malloc (out->el_size * out->total);
    
    return out;
}

void list_append (_list* ls, void* el) {
    if (ls->used >= ls->total) {
        list_realloc (ls);
    }
    
    ls->el[ls->used] = el;
    ls->used++;
}

void list_realloc (_list* ls) {
    ls->total += _LIST_INC_SIZE;
    ls->el = realloc (ls->el, ls->total * ls->el_size);
}
