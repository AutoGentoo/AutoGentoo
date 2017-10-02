/*
 * crossdev.c
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


#include <stdio.h>
#include <crossdev.h>
#include <string.h>

struct arch_select architectures[] = {
    {alpha, "alpha"},
    {aarch64, "aarch64"},
    {arm, "arm"},
    {i386, "i386"},
    {mips, "mips"},
    {mips64, "mips64"},
    {mips64el, "mips64el"},
    {mipsel, "mipsel"},
    {ppc, "ppc"},
    {ppc64, "ppc64"},
    {s390x, "s390x"},
    {sh4, "sh4"},
    {sh4eb, "sh4eb"},
    {sparc, "sparc"},
    {sparc64, "sparc64"},
    {x86_64, "x86_64"}
};

int select_architecture (int current, cross_arch arch) {
    return current | arch;
}

int deselect_architecture (int current, cross_arch arch) {
    return current ^ arch;
}

char* get_architecture_name (cross_arch arch) {
    int i;
    for (i = 0; i != sizeof(architectures) / sizeof (struct arch_select); i++) {
        if (architectures[i].arch == arch) {
            return architectures[i].str;
        }
    }
    return "";
}

cross_arch get_architecture (char* name) {
    int i;
    for (i = 0; i != sizeof(architectures) / sizeof (struct arch_select); i++) {
        if (strcmp (architectures[i].str, name) == 0) {
            return architectures[i].arch;
        }
    }
    return -1;
}