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

struct serve_client { // Chroot environment 
    char hostname[64];
    char profile[128];
    char id[16]; // 16 char id for ids
    
    // Architecture configuration
    char CFLAGS[1024];
    char CXXFLAGS[64];
    char CHOST[32];
    char USE[512];
    char EXTRA[512][32];
    int extra_c;
    
    // Portage binhost setup
    char PORTAGE_TMPDIR[256]; // build dir, relative to sc_root
    char PORTDIR[256]; // ebuild portage tree, relative to /
    char DISTDIR[256]; // distfiles, relative to sc_root
    char PKGDIR[256]; // built bins, relative to sc_root
    char PORT_LOGDIR[256]; // logs, relative to sc_root
};

struct _client {
    char CFLAGS[1024];
    char CXXFLAGS[64];
    char CHOST[32];
    char USE[128];
};

struct _iptosc {
    char ip[16];
    char id[16];
};

#define MAX_CLIENTS 32

struct manager {
    struct serve_client clients[MAX_CLIENTS];
    struct _iptosc current_actives[MAX_CLIENTS];    // Link ip to serve_client multiple to ips
                                                    // Client can link to multiple server_clients and switch accordingly
    int _iptosc_c;
    int client_c;
    int debug; // If true, will not write to config
    char root[256];
    char _config[256];
};

int get_client_from_ip (struct manager * m_man, char* ip);
int get_client_from_id (struct manager * m_man, char* ip);
void init_serve_client (struct manager m_man, struct serve_client conf);
void _mkdir(const char *dir);
void write_serve (int fd, struct manager * m_man);
void read_serve (int fd, struct manager * m_man);
int get_client_from_hostname  (struct manager * m_man, char * hostname);
void write_client (char*);
void _ip_activate (struct manager* m_man, char* ip, char* id);
void remove_client (struct manager* m_man, int index);

typedef enum {
    CREATE, // Create new serve_client
    INIT, // Initialize the new serve_client
    ACTIVATE, // Active ip to id
    GETCLIENT, // Get client information (CFLAGS, CHOST etc.)
    STAGE1, // Emerge system base packages
    UNOSYNC, // emerge -uDN @world
    UPDATE, // emerge --sync && emerge -uDN @world
    EDIT, // Edit your make.conf and serve_client configuration
    GETCLIENTS, // Not to be confused with GETCLIENTS, first return line will be client_c followed by \n delimed ids
    GETACTIVE, // Returns id of active client from the ip of request
    GETSPEC, // Returns specs about the build server
    SYNC, // Run emerge --sync
    SCREMOVE, // Id to remove
    REGEN // Regenerate libraries with emerge -q @preserved-rebuild
} serve_c;

struct link_srv {
    serve_c command;
    int argc;
};

struct str_req {
    char* ID;
    serve_c BIND;
};

extern struct link_srv link_methods [];
extern struct str_req str_link[];

#define L_CREATE (struct link_srv) {CREATE, 5}
#define L_INIT (struct link_srv) {INIT, 0}
#define L_ACTIVATE (struct link_srv) {ACTIVATE, 1} // ID of serve_client
#define L_GETCLIENT (struct link_srv) {GETCLIENT, 1} // Ask for id
#define L_STAGE1 (struct link_srv) {STAGE1, 0}
#define L_UNOSYNC (struct link_srv) {UNOSYNC, 0}
#define L_UPDATE (struct link_srv) {UPDATE, 0}
#define L_EDIT (struct link_srv) {EDIT, 6}
#define L_GETCLIENTS (struct link_srv) {GETCLIENTS, 0}
#define L_GETACTIVE (struct link_srv) {GETACTIVE, 0}
#define L_GETSPEC (struct link_srv) {GETSPEC, 0}
#define L_SYNC (struct link_srv) {SYNC, 0}
#define L_SCREMOVE (struct link_srv) {SCREMOVE, 1}
#define L_REGEN (struct link_srv) {REGEN, 0}

#define S_CREATE (struct str_req) {"CREATE", CREATE}
#define S_INIT (struct str_req) {"INIT", INIT}
#define S_ACTIVATE (struct str_req) {"ACTIVATE", ACTIVATE}
#define S_GETCLIENT (struct str_req) {"GETCLIENT", GETCLIENT}
#define S_STAGE1 (struct str_req) {"STAGE1", STAGE1}
#define S_UNOSYNC (struct str_req) {"UNOSYNC", UNOSYNC}
#define S_UPDATE (struct str_req) {"UPDATE", UPDATE}
#define S_EDIT (struct str_req) {"EDIT", EDIT}
#define S_GETCLIENTS (struct str_req) {"GETCLIENTS", GETCLIENTS}
#define S_GETACTIVE (struct str_req) {"GETACTIVE", GETACTIVE}
#define S_GETSPEC (struct str_req) {"GETSPEC", GETSPEC}
#define S_SYNC (struct str_req) {"SYNC", SYNC}
#define S_SCREMOVE (struct str_req) {"SCREMOVE", SCREMOVE}
#define S_REGEN (struct str_req) {"REGEN", REGEN}

struct link_srv get_link_srv (serve_c);
struct link_srv get_link_str (char*);

#endif
