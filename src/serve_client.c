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


#include <serve_client.h>
#include <ip_convert.h>
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
#include <request.h>
#include <_string.h>
#include <sys/stat.h>
#include <chroot.h>

static const struct serve_client EMPTY_CLIENT;

char * get_ip_from_fd (int fd) {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(fd, (struct sockaddr *)&addr, &addr_size);
    char *ip;
    ip = inet_ntoa(addr.sin_addr);
    return ip;
}

int get_client_from_ip (struct manager * m_man, char* ip) {
    int i;
    for (i=0; i!=m_man->_iptosc_c; i++) {
        if (strcmp (m_man->current_actives[i].ip, ip) == 0) {
            return get_client_from_id (m_man, m_man->current_actives[i].id);
        }
    }
    return -1;
}

int get_client_from_id (struct manager * m_man, char* id) {
    int i;
    for (i=0; i!=m_man->client_c; i++) {
        if (strcmp (m_man->clients[i].id, id) == 0) {
            return i;
        }
    }
    
    return -1;
}

int get_client_from_hostname  (struct manager * m_man, char * hostname) {
    int i;
    for (i=0; i != m_man->client_c; i++) {
        if (strcmp (m_man->clients[i].hostname, hostname) == 0) {
            return i;
        }
    }
    return -1;
}

/* Because a system is just a set of packages
 * All we need here is to setup a make.conf to emerge these packages
 */

void write_make_conf (struct manager m_man, struct serve_client conf) {
    char make_conf_buff [8192];
    char __ROOT_ [128];
    char EXTRA [2048];
    int i_c;
    for (i_c=0; i_c!=conf.extra_c; i_c++) {
        strcat (EXTRA, conf.EXTRA[i_c]);
        strcat (EXTRA, "\n");
    }
    sprintf (__ROOT_, "%s/%s/", m_man.root, conf.id);
    char* _ROOT_ = path_normalize (__ROOT_);
    
    char cbuild[64];
    FILE* cbuild_stream = popen ("gcc -dumpmachine", "r");
    if(!cbuild_stream){
        fprintf(stderr, "Could not determine CBUILD\n");
        return;
    }
    
    fgets(cbuild, 64, cbuild_stream);
    *strchr (cbuild, '\n') = '\0';
    
    sprintf (make_conf_buff, "# This file has been generated by AutoGentoo\n\
# Do NOT change any settings in this file\n\
# Changes to this file could cause both\n\
# server and client system corruption!\n\
\n\
# System architecture configuration\n\
CFLAGS=\"%s\"\n\
CXXFLAGS=\"%s\"\n\
CHOST=\"%s\"\n\
CBUILD=\"%s\"\n\
\n\
# Portage system configuration\n\
SYS_ROOT=\"%s\"\n\
PORTAGE_TMPDIR=\"${SYS_ROOT}%s\"\n\
PORTDIR=\"${SYS_ROOT}%s\"\n\
DISTDIR=\"${SYS_ROOT}%s\"\n\
PKGDIR=\"${SYS_ROOT}%s\"\n\
PORT_LOGDIR=\"%s\"\n\
\n\
# Portage package configuration\n\
USE=\"%s\"\n\
EMERGE_DEFAULT_OPTS=\"--buildpkg --usepkg --autounmask-continue\"\n\
\n\
%s\
\n",    conf.CFLAGS, conf.CXXFLAGS, conf.CHOST, cbuild,
        (conf.state >= STAGE3) ? "" : _ROOT_,// If chroot is true SYS_ROOT will be ""
        conf.PORTAGE_TMPDIR,
        conf.PORTDIR,
        conf.DISTDIR,
        conf.PKGDIR,
        conf.PORT_LOGDIR,
        conf.USE,
        EXTRA
    );
    
    char make_conf_file [256];
    sprintf (make_conf_file, "%s/etc/portage/make.conf", _ROOT_);
    FILE * fp_mc;
    
    fp_mc = fopen (make_conf_file, "w+");
    if (fp_mc != NULL)
    {
        fputs(make_conf_buff, fp_mc);
        fclose(fp_mc);
    }
}

void init_serve_client (struct manager* m_man, int sc_no) {
    struct serve_client* conf = &m_man->clients[sc_no];
    char _ROOT_ [128];
    sprintf (_ROOT_, "%s/%s/", m_man->root, conf->id);
    
    char *new_dirs [] = {
        conf->PORTAGE_TMPDIR,
        conf->PKGDIR,
        conf->PORT_LOGDIR,
        conf->PORTDIR,
        "etc/portage",
        "usr",
        "lib32",
        "lib64",
        "usr/lib32",
        "usr/lib64",
        "proc",
        "sys",
        "dev"
    };
    
    int i;
    for (i=0; i!=sizeof (new_dirs) / sizeof (*new_dirs); i++) {
        char TARGET_DIR[256];
        sprintf (TARGET_DIR, "%s/%s", _ROOT_, new_dirs[i]);
        _mkdir(TARGET_DIR);
    }
    
    write_make_conf (*m_man, *conf);
    
    pid_t link_fork = fork();
    if (link_fork == -1) {
        exit (-1);
    }
    if (link_fork == 0) {
        chdir (_ROOT_);
        
        // Create the profile symlink
        char sym_buf_p1 [128];
        char sym_buf_p2 [128];
        sprintf (sym_buf_p1, "/usr/portage/profiles/%s/", conf->profile);
        strcpy (sym_buf_p2, "./etc/portage/make.profile");
        char sym_lib_p1[128];
        char sym_lib_p2[128];
        strcpy (sym_lib_p2, "./lib");
        
        // Check if symlinks exist and remove them
        struct stat __sym_buff;
        if (lstat (sym_buf_p2, &__sym_buff) == 0) {
            unlink (sym_buf_p2);
        }
        if (lstat (sym_lib_p2, &__sym_buff) == 0) {
            unlink (sym_lib_p2);
        }
        
        if (symlink (sym_buf_p1, sym_buf_p2) != 0) {
            printf ("Failed to symlink profile!\n");
        }
        if ((size_t)-1 > 0xffffffffUL) {
            strcpy (sym_lib_p1, "./lib64");
        }
        else {
            strcpy (sym_lib_p1, "./lib32");
        }
        if (symlink (sym_lib_p1, sym_lib_p2) != 0) {
            printf ("Failed to symlink /lib!\n");
        }
        strcpy (sym_lib_p2, "./usr/lib");
        if (lstat (sym_lib_p2, &__sym_buff) == 0) { // Again for /usr/lib
            unlink (sym_lib_p2);
        }
        if ((size_t)-1 > 0xffffffffUL) {
            strcpy (sym_lib_p1, "./lib64");
        }
        else {
            strcpy (sym_lib_p1, "./lib32");
        }
        if (symlink (sym_lib_p1, sym_lib_p2) != 0) {
            printf ("Failed to symlink /usr/lib!\n");
        }
        
        exit (0);
    }
    
    int link_return;
    waitpid (link_fork, &link_return, 0); // Wait until finished
}

void _mkdir(const char *dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if(tmp[len - 1] == '/')
            tmp[len - 1] = 0;
    for(p = tmp + 1; *p; p++) {
        if(*p == '/') {
            *p = 0;
            mkdir(tmp, 0777);
            *p = '/';
        }
    }
    mkdir(tmp, 0777);
}

void write_serve (int fd, struct manager * m_man) {
    write (fd, m_man, sizeof (struct manager));
}

void read_serve (int fd, struct manager * m_man) { // You must malloc this pointer first
    read (fd, m_man, sizeof(struct manager));
    int i;
    for (i=0; i!=m_man->client_c; i++) {
        if (m_man->clients[i].state >= STAGE3) {
            m_man->clients[i].chroot = chroot_new (m_man, i);
            chroot_mount (m_man->clients[i].chroot);
            m_man->clients[i].state = STAGE3;
        }
    }
}

struct link_srv link_methods [] = {
    L_CREATE,
    L_INIT,
    L_ACTIVATE,
    L_GETCLIENT,
    L_STAGE1,
    L_EDIT,
    L_GETCLIENTS,
    L_GETACTIVE,
    L_GETSPEC,
    L_SYNC,
    L_SCREMOVE,
    L_MNTCHROOT,
    L_DEVCREATE
};

struct link_srv get_link_srv (serve_c c) {
    int i;
    for (i=0; i!=sizeof (link_methods) / sizeof (struct link_srv); i++) {
        if (c == link_methods[i].command)
            return link_methods[i];
    }
    return (struct link_srv) {c, 0}; // command out of range
}

struct link_srv get_link_str (char* s) {
    int i;
    for (i=0; i!=sizeof(link_methods) / sizeof (struct link_srv); i++) {
        if (strcmp (s, link_methods[i].ID) == 0) {
            return link_methods[i];
        }
    }
    return (struct link_srv) {-1, 0}; // Invalid but go anyway
}

void write_client (char *ip) {
    struct _client client;
    char *client_config = "/etc/portage/autogentoo.conf";
    int fd = open (client_config, O_RDWR);
    if (access (client_config, F_OK) == -1) {
        _mkdir ("/etc/portage");
        char message[1024];
        //ask_server (ip, "SRV 3 HTTP/1.0\n", &message[0]);
        printf ("%s\n", message);
        strtok (message, "\n");
        strcpy(client.CFLAGS, strtok (NULL, "\n"));
        strcpy(client.CXXFLAGS, strtok (NULL, "\n"));
        strcpy(client.CHOST, strtok (NULL, "\n"));
        strcpy(client.USE, strtok (NULL, "\n"));
        write (fd, &client, sizeof (struct _client));
    }
    else {
        read (fd, &client, sizeof (struct _client));
    }
    close (fd);
    fd = open ("/etc/portage/make.conf", O_RDWR);
    char make_conf_buff [4092];
    sprintf (make_conf_buff, "# This file has been generated by AutoGentoo\n\
# Do NOT change any settings in this file\n\
# Changes to this file could cause both\n\
# server and client system corruption!\n\
\n\
# System architecture configuration\n\
CFLAGS=\"%s\"\n\
CXXFLAGS=\"%s\"\n\
CHOST=\"%s\"\n\
\n\
# Portage package configuration\n\
USE=\"%s\"\n\
PORTAGE_BINHOST='http://%s:9490/\n\
\n", client.CFLAGS, client.CXXFLAGS, client.CHOST, client.USE, ip);
    write (fd, make_conf_buff, strlen (make_conf_buff));
    close (fd);
}

void _ip_activate (struct manager* m_man, char* ip, char* id) {
    // Get index of ip
    int i;
    int index = -1;
    for (i=0; i!=m_man->_iptosc_c; i++) {
        if (strcmp (m_man->current_actives[i].ip, ip) == 0) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        index = m_man->_iptosc_c;
        strcpy (m_man->current_actives[index].ip, ip); // Only copy if it doesn't exist yet
        m_man->_iptosc_c++;
    }
    strcpy (m_man->current_actives[index].id, id);
}

void remove_client (struct manager* m_man, int index) {
    if (index != m_man->client_c - 1) {
        m_man->clients[index] = m_man->clients[m_man->client_c-1]; // Put the final element in its place
    }
    else {
        m_man->clients[index] = EMPTY_CLIENT; // Set as to empty item
    }
    m_man->client_c--;
}
