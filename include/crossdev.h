/*
 * crossdev.h
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


#ifndef __AUTOGENTOO_CROSSDEV_H__
#define __AUTOGENTOO_CROSSDEV_H__

typedef enum {
    alpha = 0x1,
    aarch64 = 0x2,
    arm = 0x4,
    i386 = 0x8,
    mips = 0x10,
    mips64 = 0x20,
    mips64el = 0x40,
    mipsel = 0x80,
    ppc = 0x100,
    ppc64 = 0x200,
    s390x = 0x400,
    sh4 = 0x800,
    sh4eb = 0x1000,
    sparc = 0x2000,
    sparc64 = 0x4000,
    x86_64 = 0x8000
} cross_arch;

int select_architecture (int current, cross_arch arch);
int deselect_architecture (int current, cross_arch arch);
char* get_architecture_name (cross_arch arch);

struct arch_select {
    cross_arch arch;
    char str[16];
};

extern struct arch_select architectures[];

#endif