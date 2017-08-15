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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct serve_client { // Chroot environment 
    char hostname[128];
    char profile[256];
    struct make_conf config;
};

struct serve_client_manager {
    char top_dir[256];
    struct serve_client * clients;
    int size;
    int used;
};

struct sserve_client_manager {
    char top_dir [256];
    int size;
    int used;
};

int get_client_from_host (struct serve_client_manager manager, char* hostname);
int get_client_from_ip (struct serve_client_manager manager, char* ip);
void add_to_manager (struct serve_client_manager * manager, struct serve_client conf);
void init_serve_client (struct serve_client_manager manager, struct serve_client conf);
struct serve_client_manager init_manager (char * top_dir);
void _mkdir(const char *dir);
struct sserve_client gen_simple (struct serve_client);
struct sserve_client_manager gen_simple_manager (struct serve_client_manager manager);
void write_manager (int fd, struct serve_client_manager manager);
void read_manager (int fd, struct serve_client_manager * manager);
struct serve_client read_client (int fd);

#endif
