/*
 * serve_client.c
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


#ifndef __AUTOGENTOO_KERNEL_H__
#define __AUTOGENTOO_KERNEL_H__

#include <stdio.h>
#include <hash.h>

struct kernel;
struct kconfig;
struct kbinary;

struct kconfig {
    char config_path[256];
    struct kernel parent_kernel;
}

struct kbinary {
    unsigned char sha256_hash[SHA256_DIGEST_LENGTH];
    size_t binary_size;
    char binary_path[256];
    struct kernel parent_kernel;
}

struct kernel {
    char name[32]; // suffix of version (gentoo, none for vanilla)
    char version[32];
    spec kspec; // Index of spec_list
};

struct chost {
    char arch[16];
    char vendor[16];
    char os[32];
    char c_lib[32];
};

typedef enum spec {
    i386,
    i486,
    i586,
    i686,
    x86_64,
    arm,
    armv4,
    armv4t,
    armv5te,
    armv6j,
    armv7a
};

//extern struct machine_spec spec_list[];

#endif