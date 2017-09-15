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

struct OLD_CLIENT {
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

struct old_manager {
    struct OLD_CLIENT clients[MAX_CLIENTS];
    struct _iptosc current_actives[MAX_CLIENTS];    // Link ip to serve_client multiple to ips
                                                    // Client can link to multiple server_clients and switch accordingly
    int _iptosc_c;
    int client_c;
    int debug; // If true, will not write to config
    char root[256];
    char _config[256];
};

void update_manager (struct old_manager* oman, struct manager* nman) {
    int i;
    for (i=0; i!=oman->client_c; i++) {
        strcpy (nman->clients[i].hostname, oman->clients[i].hostname);
        strcpy (nman->clients[i].profile, oman->clients[i].profile);
        strcpy (nman->clients[i].id, oman->clients[i].id);
        strcpy (nman->clients[i].CFLAGS, oman->clients[i].CFLAGS);
        strcpy (nman->clients[i].USE, oman->clients[i].USE);
        
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
    
    for (i=0; i!=oman->_iptosc_c; i++) {
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
    
    int src_fd = open (src_file, O_RDONLY);
    int dest_fd = open (dest_file, O_RDWR | O_CREAT | O_TRUNC, 0664);
    
    struct old_manager m_man_old;
    struct manager m_man_new;
    
    read (src_fd, &m_man_old, sizeof(struct old_manager));
    
    update_manager (&m_man_old, &m_man_new);
    
    
    write_serve (dest_fd, &m_man_new);
    close (dest_fd);
    
    return 0;
}