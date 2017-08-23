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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <response.h>
#include <_string.h>

struct serve_client { // Chroot environment 
    char hostname[64];
    char profile[32];
    char ip[16][16]; // Multiple ip's can point to one client
    int ip_c;
    
    // Architecture configuration
    char CFLAGS[128];
    char CXXFLAGS[64];
    char CHOST[32];
    char USE[128];
    
    // Portage binhost setup
    char PORTAGE_TMPDIR[256]; // build dir, relative to sc_root
    char PORTDIR[256]; // ebuild portage tree, relative to /
    char DISTDIR[256]; // distfiles, relative to sc_root
    char PKGDIR[256]; // built bins, relative to sc_root
    char PORT_LOGDIR[256]; // logs, relative to sc_root
};

struct _client {
    char CFLAGS[128];
    char CXXFLAGS[64];
    char CHOST[32];
    char USE[128];
};

struct manager {
    struct serve_client clients[32];
    int client_c;
    char root[256];
    char _config[256];
};

int get_client_from_ip (struct manager * m_man, char* ip);
void init_serve_client (struct manager m_man, struct serve_client conf);
void _mkdir(const char *dir);
void write_serve (int fd, struct manager * m_man);
void read_serve (int fd, struct manager * m_man);
int get_client_from_hostname  (struct manager * m_man, char * hostname);

typedef enum {
    CREATE, // Create new serve_client
    INIT, // Initialize the new serve_client
    ADDIP, // Add ip to serve_client ip list
    GETCLIENT // Get client information (CFLAGS, CHOST etc.)
} serve_c;

struct link_srv {
    serve_c command;
    int argc;
};

extern struct link_srv link_methods [];

#define L_CREATE (struct link_srv) {CREATE, 5}
#define L_INIT (struct link_srv) {INIT, 0}
#define L_ADDIP (struct link_srv) {ADDIP, 1} // Hostname of serve_client
#define L_GETCLIENT (struct link_srv) {GETCLIENT, 0} // Automatically detect which client from ip

struct link_srv get_link_srv (serve_c);

#endif
