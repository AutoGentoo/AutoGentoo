/*
 * update-config.c
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
#include <serve_client.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

struct client_v1 {
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
    
    // Portage binhost setup (make.conf)
    char PORTAGE_TMPDIR[256]; // build dir
    char PORTDIR[256]; // ebuild portage tree
    char DISTDIR[256]; // distfiles
    char PKGDIR[256]; // built bins
    char PORT_LOGDIR[256]; // logs
};

struct manager_v1 {
    struct client_v1 clients[MAX_CLIENTS];
    struct _iptosc current_actives[MAX_CLIENTS];    // Link ip to serve_client multiple to ips
                                                    // Client can link to multiple server_clients and switch accordingly
    int _iptosc_c;
    int client_c;
    int debug; // If true, will not write to config
    char root[256];
    char _config[256];
};

struct client_v2 { // Chroot environment
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

struct manager_v2 {
    struct client_v2 clients[MAX_CLIENTS];
    struct _iptosc current_actives[MAX_CLIENTS];    // Link ip to serve_client multiple to ips
                                                    // Client can link to multiple server_clients and switch accordingly
    int _iptosc_c;
    int client_c;
    int debug; // If true, will not write to config
    char root[256];
    char _config[256];
};

void update_manager_v1 (struct manager_v1* oman, struct manager* nman) {
    int i;
    for (i = 0; i != oman->client_c; i++) {
        strcpy (nman->clients[i].hostname, oman->clients[i].hostname);
        strcpy (nman->clients[i].profile, oman->clients[i].profile);
        strcpy (nman->clients[i].id, oman->clients[i].id);
        strcpy (nman->clients[i].CFLAGS, oman->clients[i].CFLAGS);
        strcpy (nman->clients[i].USE, oman->clients[i].USE);
        strcpy (nman->clients[i].CHOST, oman->clients[i].CHOST);
        strcpy (nman->clients[i].CXXFLAGS, oman->clients[i].CXXFLAGS);
        
        int j;
        for (j=0; j!=oman->clients[i].extra_c; j++) {
            strcpy (nman->clients[i].EXTRA[j], oman->clients[j].EXTRA[j]);
        }
        nman->clients[i].extra_c = oman->clients[i].extra_c;
        
        strcpy (nman->clients[i].PORTAGE_TMPDIR, oman->clients[i].PORTAGE_TMPDIR);
        strcpy (nman->clients[i].PORTDIR, oman->clients[i].PORTDIR);
        strcpy (nman->clients[i].DISTDIR, oman->clients[i].DISTDIR);
        strcpy (nman->clients[i].PKGDIR, oman->clients[i].PKGDIR);
        strcpy (nman->clients[i].PORT_LOGDIR, oman->clients[i].PORT_LOGDIR);
        
        strcpy (nman->clients[i].PORTAGE_DIR, "/usr/portage");
        strcpy (nman->clients[i].resolv_conf, "/etc/resolv.conf");
        strcpy (nman->clients[i].locale, "en_US.utf8");
    }
    nman->client_c = oman->client_c;
    
    for (i = 0; i != oman->_iptosc_c; i++) {
        strcpy (nman->current_actives[i].id, oman->current_actives[i].id);
        strcpy (nman->current_actives[i].ip, oman->current_actives[i].ip);
    }
    
    nman->_iptosc_c = oman->_iptosc_c;
    nman->debug = oman->debug;
    
    strcpy (nman->root, oman->root);
    strcpy (nman->_config, oman->_config);
    
    fflush (stdout);
}
void update_manager_v2 (struct manager_v2* oman, struct manager* nman) {
    int i;
    for (i = 0; i != oman->client_c; i++) {
        strcpy (nman->clients[i].hostname, oman->clients[i].hostname);
        strcpy (nman->clients[i].profile, oman->clients[i].profile);
        strcpy (nman->clients[i].id, oman->clients[i].id);
        strcpy (nman->clients[i].CFLAGS, oman->clients[i].CFLAGS);
        strcpy (nman->clients[i].USE, oman->clients[i].USE);
        strcpy (nman->clients[i].CHOST, oman->clients[i].CHOST);
        strcpy (nman->clients[i].CXXFLAGS, oman->clients[i].CXXFLAGS);
        
        int j;
        for (j=0; j!=oman->clients[i].extra_c; j++) {
            strcpy (nman->clients[i].EXTRA[j], oman->clients[j].EXTRA[j]);
        }
        nman->clients[i].extra_c = oman->clients[i].extra_c;
        
        strcpy (nman->clients[i].PORTAGE_TMPDIR, oman->clients[i].PORTAGE_TMPDIR);
        strcpy (nman->clients[i].PORTDIR, oman->clients[i].PORTDIR);
        strcpy (nman->clients[i].DISTDIR, oman->clients[i].DISTDIR);
        strcpy (nman->clients[i].PKGDIR, oman->clients[i].PKGDIR);
        strcpy (nman->clients[i].PORT_LOGDIR, oman->clients[i].PORT_LOGDIR);
        
        strcpy (nman->clients[i].PORTAGE_DIR, "/usr/portage");
        strcpy (nman->clients[i].resolv_conf, "/etc/resolv.conf");
        strcpy (nman->clients[i].locale, "en_US.utf8");
    }
    nman->client_c = oman->client_c;
    
    for (i = 0; i != oman->_iptosc_c; i++) {
        strcpy (nman->current_actives[i].id, oman->current_actives[i].id);
        strcpy (nman->current_actives[i].ip, oman->current_actives[i].ip);
    }
    
    nman->_iptosc_c = oman->_iptosc_c;
    nman->debug = oman->debug;
    
    strcpy (nman->root, oman->root);
    strcpy (nman->_config, oman->_config);
    
    fflush (stdout);
}

int main (int argc, char** argv) {
    char* src_file = argv[1];
    char* dest_file = argv[2];
    char* old_version = argv[3];
    
    int src_fd = open (src_file, O_RDONLY);
    int dest_fd = open (dest_file, O_RDWR | O_CREAT | O_TRUNC, 0664);
    
    struct manager m_man_new;
    void* m_man_old;
    size_t old_size;
    
    if (strncmp (old_version, "1\0", 2) == 0) {
        old_size = sizeof (struct manager_v1);
        m_man_old = malloc (old_size);
        update_manager_v1 (m_man_old, &m_man_new);
    }
    else if (strncmp (old_version, "2\0", 2) == 0) {
        old_size = sizeof (struct manager_v2);
        m_man_old = malloc (old_size);
        update_manager_v2 (m_man_old, &m_man_new);
    }
    
    
    read (src_fd, m_man_old, old_size);
    
    write_serve (dest_fd, &m_man_new);
    
    close (dest_fd);
    free (m_man_old);
    
    return 0;
}