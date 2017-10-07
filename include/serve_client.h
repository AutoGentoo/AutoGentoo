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
    
    struct kernel_client* kernel;
};

struct _iptosc {
    char ip[16];
    char id[16];
};

#ifndef MAX_CLIENTS
#define MAX_CLIENTS 32
#endif

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
void _ip_activate (struct manager* m_man, char* ip, char* id);
void remove_client (struct manager* m_man, int index);

#endif
