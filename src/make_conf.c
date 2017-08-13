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


#include <make_conf.h>

struct make_conf init_make_conf (char * CHOST, char * CFLAGS, char * USE) {
    struct make_conf out;
    out.CHOST = CHOST;
    out.CFLAGS = CFLAGS;
    out.CXXFLAGS = "${CFLAGS}";
    out.USE = USE;
    
    out.PORTAGE_TMPDIR = "autogentoo/tmp";
    out.PORTDIR = "/usr/portage";
    out.DISTDIR = "autogentoo/dist";
    out.PKGDIR = "autogentoo/pkg";
    out.PORT_LOGDIR = "autogentoo/log";
    
    return out;
}
