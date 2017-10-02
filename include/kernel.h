/*
 * kernel.h
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
#include <crossdev.h>
#include <serve_client.h>
#include <response.h>
#include <openssl/sha.h>

struct kernel;
struct kbinary;

struct kbinary {
    unsigned char sha256_hash[SHA256_DIGEST_LENGTH];
    size_t binary_size;
    char binary_path[256];
    char id[16]; // Serve client ID
};

struct kernel_client {
    char release[32];
    char portage_arch[16];
    char chost[32];
    cross_arch arch;
};

struct kernel_client* init_kernel (struct manager* m_man, int sc_no, char* architecture);

response_t kernel_config (struct manager* m_man, int sc_no);
response_t kernel_build (struct manager* m_man, int sc_no);

#endif