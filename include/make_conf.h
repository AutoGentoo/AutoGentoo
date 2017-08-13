/*
 * make_conf.c
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


#ifndef __AUTOGENTOO_MAKE_CONF__
#define __AUTOGENTOO_MAKE_CONF__

#include <stdio.h>
#include <string.h>

struct make_conf {
    // Architecture configuration
    char * CFLAGS;
    char * CXXFLAGS;
    char * CHOST;
    char * USE;
    
    // Portage binhost setup
    char * PORTAGE_TMPDIR; // build dir, relative to sc_root
    char * PORTDIR; // ebuild portage tree, relative to /
    char * DISTDIR; // distfiles, relative to sc_root
    char * PKGDIR; // built bins, relative to sc_root
    char * PORT_LOGDIR; // logs, relative to sc_root
};

struct make_conf init_make_conf (char * CHOST, char * CFLAGS, char * USE);

#endif
