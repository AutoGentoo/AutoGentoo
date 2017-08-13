/*
 * serve_client.h
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

#ifndef __AUTOGENTOO_SERVE_CLIENT__
#define __AUTOGENTOO_SERVE_CLIENT__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <make_conf.h>
#include <sys/stat.h>
#include <sys/types.h>

struct serve_client { // Chroot environment 
    char * hostname;
    struct make_conf config;
    struct serve_client * back;
    struct serve_client * next;
};

struct serve_client_manager {
    char * top_dir;
    struct serve_client ** clients;
    int size;
    int used;
};

struct serve_client * get_client_from_host (struct serve_client_manager * manager, char* hostname);
struct serve_client * get_client_from_ip (struct serve_client_manager * manager, char* ip);
void add_to_manager (struct serve_client_manager * manager, struct serve_client conf);
void init_serve_client (struct serve_client_manager * manager, struct serve_client conf);
struct serve_client_manager * init_manager (char * top_dir);
void _mkdir(const char *dir);

#endif
