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

typedef enum {
    NO_INIT, // Directories and make.conf need creation
    INITED, // Directories have been created but base packages need installation
    STAGE3, /* Stage3 packages have been installed (chroot_ready).
             * After every restart the state will reset here if higher
             */
    CHROOT // Directories mounted and ready and chroot is running (run_state)
} client_state;

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
    
    // CHROOT setup
    char PORTAGE_DIR[128]; // Usually /usr/portage, different if you have an overlay
    char resolv_conf[64]; // Almost always /etc/resolv.conf
    char locale[16]; // UTF-8 is recommended/default
    
    // Portage binhost setup (make.conf)
    char PORTAGE_TMPDIR[256]; // build dir
    char PORTDIR[256]; // ebuild portage tree
    char DISTDIR[256]; // distfiles
    char PKGDIR[256]; // built bins
    char PORT_LOGDIR[256]; // logs

    struct chroot_client* chroot;
    client_state state;
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

char * get_ip_from_fd (int);
int get_client_from_ip (struct manager * m_man, char* ip);
int get_client_from_id (struct manager * m_man, char* ip);
void write_make_conf (struct manager m_man, struct serve_client conf);
void init_serve_client (struct manager* m_man, int sc_no);
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
    EDIT, // Edit your make.conf and serve_client configuration
    GETCLIENTS, // Not to be confused with GETCLIENTS, first return line will be client_c followed by \n delimed ids
    GETACTIVE, // Returns id of active client from the ip of request
    GETSPEC, // Returns specs about the build server
    SYNC, // Run emerge --sync
    SCREMOVE, // Id to remove
    MNTCHROOT // Mount chroot directories and start the thread
} serve_c;

struct link_srv {
    serve_c command;
    int argc;
    char* ID;
};

extern struct link_srv link_methods [];

#define L_CREATE (struct link_srv) {CREATE, 5, "CREATE"}
#define L_INIT (struct link_srv) {INIT, 0, "INIT"}
#define L_ACTIVATE (struct link_srv) {ACTIVATE, 1, "ACTIVATE"} // ID of serve_client
#define L_GETCLIENT (struct link_srv) {GETCLIENT, 1, "GETCLIENT"} // Ask for id
#define L_STAGE1 (struct link_srv) {STAGE1, 0, "STAGE1"}
#define L_EDIT (struct link_srv) {EDIT, 6, "EDIT"}
#define L_GETCLIENTS (struct link_srv) {GETCLIENTS, 0, "GETCLIENTS"}
#define L_GETACTIVE (struct link_srv) {GETACTIVE, 0, "GETACTIVE"}
#define L_GETSPEC (struct link_srv) {GETSPEC, 0, "GETSPEC"}
#define L_SYNC (struct link_srv) {SYNC, 0, "SYNC"}
#define L_SCREMOVE (struct link_srv) {SCREMOVE, 1, "SCREMOVE"}
#define L_MNTCHROOT (struct link_srv) {MNTCHROOT, 0, "MNTCHROOT"}

struct link_srv get_link_srv (serve_c);
struct link_srv get_link_str (char*);

#endif
