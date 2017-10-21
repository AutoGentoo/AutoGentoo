/*
 * srv_handle.c
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

#include <srv_handle.h>
#include <chroot.h>
#include <_string.h>
#include <kernel.h>

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
    L_DEVCREATE,
    L_HKBUILD,
    L_GKBUILD,
    L_GETKERNEL
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

struct srv_f_link srv_methods[] = {
    {CREATE, _CREATE},
    {ACTIVATE, _ACTIVATE},
    {INIT, _INIT},
    {GETCLIENT, _GETCLIENT},
    {STAGE1, _STAGE1},
    {SYNC, _SYNC},
    {EDIT, _EDIT},
    {GETCLIENTS, _GETCLIENTS},
    {GETACTIVE, _GETACTIVE},
    {GETSPEC, _GETSPEC},
    {SCREMOVE, _SCREMOVE},
    {MNTCHROOT, _MNTCHROOT},
    {DEVCREATE, _DEVCREATE},
    {HKBUILD, _HKBUILD},
    {GKBUILD, _GKBUILD},
    {GETKERNEL, _GETKERNEL}
};

void update_config (struct manager* m_man) {
    if(!m_man->debug) {
        FILE * _fd = fopen (m_man->_config, "w+");
        write_serve (fileno(_fd), m_man);
        fclose (_fd);
    }
}

response_t srv_handle (int sockfd, request_t type, struct manager* m_man, char** args, int n) {
    int i;
    char* ip = get_ip_from_fd (sockfd);
    response_t res = METHOD_NOT_ALLOWED;
    for (i = 0; i != sizeof (srv_methods) / sizeof (struct srv_f_link); i++) {
        if (type == srv_methods[i].type) {
            res = srv_methods[i].func (m_man, ip, sockfd, args, n);
        }
    }
    
    rsend (sockfd, res);
    return res;
}

response_t _CREATE (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    strcpy(m_man->clients[m_man->client_c].hostname, args[0]);
    strcpy(m_man->clients[m_man->client_c].profile, args[1]);
    strcpy(m_man->clients[m_man->client_c].CHOST, args[2]);
    strcpy(m_man->clients[m_man->client_c].CFLAGS, args[3]);
    strcpy(m_man->clients[m_man->client_c].CXXFLAGS, "${CFLAGS}");
    strcpy(m_man->clients[m_man->client_c].USE, args[4]);
    for (m_man->clients[m_man->client_c].extra_c=0; m_man->clients[m_man->client_c].extra_c != n - 5; m_man->clients[m_man->client_c].extra_c++) {
        strcpy (m_man->clients[m_man->client_c].EXTRA[m_man->clients[m_man->client_c].extra_c], args[m_man->clients[m_man->client_c].extra_c+5]);
    }
    strcpy(m_man->clients[m_man->client_c].PORTAGE_TMPDIR, "/autogentoo/tmp");
    strcpy(m_man->clients[m_man->client_c].PORTDIR, "/usr/portage");
    strcpy(m_man->clients[m_man->client_c].DISTDIR, "/usr/portage/distfiles");
    strcpy(m_man->clients[m_man->client_c].PKGDIR, "/autogentoo/pkg");
    strcpy(m_man->clients[m_man->client_c].PORT_LOGDIR, "/autogentoo/log");
    strcpy (m_man->clients[m_man->client_c].PORTAGE_DIR, "/usr/portage");
    strcpy (m_man->clients[m_man->client_c].resolv_conf, "/etc/resolv.conf");
    strcpy (m_man->clients[m_man->client_c].locale, "en_US.utf8");
    
    gen_id (m_man->clients[m_man->client_c].id, 14); // Leave extra space for buf and 1 for \0
    
    _ip_activate (m_man, ip, m_man->clients[m_man->client_c].id);
    m_man->client_c++;
    update_config (m_man);
    
    write (sockfd, m_man->clients[m_man->client_c - 1].id, strlen(m_man->clients[m_man->client_c - 1].id));
    write (sockfd, "\n", 1);
    
    return OK;
}

response_t _ACTIVATE (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    int sc_no = get_client_from_id (m_man, args[0]);
    if (sc_no == -1) {
        return BAD_REQUEST;
    }
    else {
        _ip_activate (m_man, ip, m_man->clients[sc_no].id);
    }
    
    update_config (m_man);
    
    return OK;
}

response_t _INIT (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    int sc_no = get_client_from_ip (m_man, ip);
    if (sc_no == -1) {
        return FORBIDDEN;
    }
    
    init_serve_client (m_man, sc_no);
    return OK;
}

response_t _GETCLIENT (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    int sc_no = get_client_from_id (m_man, args[0]);
    
    if (sc_no == -1) {
        return NOT_FOUND;
    }
    
    char c_buff[4196];
    char EXTRA [2048];
    
    sprintf (EXTRA, "");
    
    int i_c;
    for (i_c=0; i_c!=m_man->clients[sc_no].extra_c; i_c++) {
        strcat (EXTRA, m_man->clients[sc_no].EXTRA[i_c]);
        strcat (EXTRA, "\n");
    }
    sprintf (c_buff, "%d\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
             m_man->clients[sc_no].extra_c,
             m_man->clients[sc_no].CFLAGS,
             m_man->clients[sc_no].CXXFLAGS,
             m_man->clients[sc_no].CHOST,
             m_man->clients[sc_no].USE,
             m_man->clients[sc_no].hostname,
             m_man->clients[sc_no].profile,
             EXTRA);
    write (sockfd, c_buff, strlen (c_buff));
    return OK;
}

response_t _STAGE1 (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    int sc_no = get_client_from_ip (m_man, ip);
    if (sc_no == -1) {
        return FORBIDDEN;
    }
    char pkgs[8192];
    FILE * fp = fopen ("/usr/portage/profiles/default/linux/packages.build", "r");
    char line[255];
    strcat (pkgs, "-e "); // Emerge the entire tree
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        char *pos;
        if ((pos=strchr(line, '\n')) != NULL)
            *pos = '\0';
        if (line[0] == '#' || line[0] == '\n' || strcmp (line, "") == 0) {
            continue;
        }
        strcat (pkgs, line);
        strcat (pkgs, " ");
    }
    fclose (fp);
    
    response_t res = __m_install (pkgs, m_man, sc_no, ip, 1);
    m_man->clients[sc_no].state = STAGE3;
    
    m_man->clients[sc_no].chroot = chroot_new (m_man, sc_no);
    chroot_mount (m_man->clients[sc_no].chroot);
    
    update_config (m_man);
    
    return res;
}

response_t _SYNC (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    return system("emerge --sync") == 0 ? OK : INTERNAL_ERROR;
}

response_t _EDIT (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    int sc_no = get_client_from_id (m_man, args[0]);
    if (sc_no == -1) {
        return NOT_FOUND;
    }
    
    strcpy(m_man->clients[sc_no].hostname, args[1]);
    strcpy(m_man->clients[sc_no].profile, args[2]);
    strcpy(m_man->clients[sc_no].CHOST, args[3]);
    strcpy(m_man->clients[sc_no].CFLAGS, args[4]);
    strcpy(m_man->clients[sc_no].CXXFLAGS, "${CFLAGS}");
    strcpy(m_man->clients[sc_no].USE, args[5]);
    for (m_man->clients[sc_no].extra_c=0; m_man->clients[sc_no].extra_c != n - 6;m_man->clients[sc_no].extra_c++) {
        strcpy (m_man->clients[sc_no].EXTRA[m_man->clients[sc_no].extra_c], args[m_man->clients[sc_no].extra_c+6]);
    }
    strcpy(m_man->clients[sc_no].PORTAGE_TMPDIR, "/autogentoo/tmp");
    strcpy(m_man->clients[sc_no].PORTDIR, "/usr/portage");
    strcpy(m_man->clients[sc_no].DISTDIR, "/usr/portage/distfiles");
    strcpy(m_man->clients[sc_no].PKGDIR, "/autogentoo/pkg");
    strcpy(m_man->clients[sc_no].PORT_LOGDIR, "/autogentoo/log");
    write_make_conf (*m_man, m_man->clients[sc_no]);
    
    update_config (m_man);
}

response_t _GETCLIENTS (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    char tmp[12]={0x0};
    sprintf(tmp,"%d", m_man->client_c);
    write(sockfd, tmp, strlen(tmp));
    write (sockfd, "\n", sizeof(char));
    int i;
    for (i=0; i!=m_man->client_c; i++) {
        write (sockfd, m_man->clients[i].id, 14);
        write (sockfd, "\n", sizeof(char));
    }
    
    return OK;
}

response_t _GETACTIVE (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    int sc_no = get_client_from_ip (m_man, ip);
    if (sc_no == -1) {
        write (sockfd, "invalid\n", 8);
        return UNAUTHORIZED;
    }
    
    write (sockfd, m_man->clients[sc_no].id, strlen(m_man->clients[sc_no].id));
    write (sockfd, "\n", 1);
    
    return OK;
}

response_t _GETSPEC (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    system ("lscpu > build.spec");
    FILE *lspcu_fp = fopen("build.spec", "r");
    char symbol;
    if(lspcu_fp != NULL)
    {
        while((symbol = getc(lspcu_fp)) != EOF)
        {
            write (sockfd, &symbol, sizeof (char));
        }
        fclose(lspcu_fp);
        remove ("build.spec");
    }
    
    return OK;
}
response_t _SCREMOVE (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    int sc_no = get_client_from_id (m_man, args[0]);
    if (sc_no == -1) {
        return NOT_FOUND;
    }
    remove_client (m_man, sc_no);
    
    update_config (m_man);
    return OK;
}
response_t _MNTCHROOT (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    int sc_no = get_client_from_ip (m_man, ip);
    if (sc_no == -1) {
        return FORBIDDEN;
    }
    
    int d = 8;
    print_bin(&d, sizeof(int));
    return OK;
    
    
    if (m_man->clients[sc_no].chroot == NULL) {
        m_man->clients[sc_no].chroot = chroot_new (m_man, sc_no);
    }
    
    chroot_mount (m_man->clients[sc_no].chroot);
    
    update_config (m_man);
    return OK;
}

response_t _DEVCREATE (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    int sc_no = get_client_from_id (m_man, args[0]);
    struct serve_client* current_client;
    if (sc_no == -1) {
        current_client = &(m_man->clients[m_man->client_c]);
        m_man->client_c++;
    }
    else {
        current_client = &(m_man->clients[sc_no]);
    }
    
    printf ("client: %p\nm_man: %p\n", current_client, m_man);
    
    strncpy (current_client->id, args[0], 14);
    
    strcpy(current_client->hostname, args[1]);
    strcpy(current_client->profile, args[2]);
    strcpy(current_client->CHOST, args[3]);
    strcpy(current_client->CFLAGS, args[4]);
    strcpy(current_client->CXXFLAGS, "${CFLAGS}");
    strcpy(current_client->USE, args[5]);
    
    strcpy(current_client->PORTAGE_TMPDIR, "/autogentoo/tmp");
    strcpy(current_client->PORTDIR, "/usr/portage");
    strcpy(current_client->DISTDIR, "/usr/portage/distfiles");
    strcpy(current_client->PKGDIR, "/autogentoo/pkg");
    strcpy(current_client->PORT_LOGDIR, "/autogentoo/log");
    strcpy (current_client->PORTAGE_DIR, "/usr/portage");
    strcpy (current_client->resolv_conf, "/etc/resolv.conf");
    strcpy (current_client->locale, "en_US.utf8");
    
    for (current_client->extra_c=0; current_client->extra_c != n - 6; current_client->extra_c++) {
        strcpy (current_client->EXTRA[current_client->extra_c], args[current_client->extra_c+6]);
    }
    
    update_config (m_man);
    
    write (sockfd, current_client->id, strlen(current_client->id));
    write (sockfd, "\n", 1);
    
    return OK;
}

// HEAD for the built kernel binary
response_t _HKBUILD (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    int sc_no = get_client_from_ip (m_man, ip);
    if (sc_no == -1) {
        return NOT_FOUND;
    }
    
    return OK;
}

// The built kernel binary
response_t _GKBUILD (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    int sc_no = get_client_from_ip (m_man, ip);
    if (sc_no == -1) {
        return NOT_FOUND;
    }
    
    struct serve_client* current_client = &(m_man->clients[sc_no]);
    if (current_client->kernel == NULL) {
        char new_arch[32];
        
        current_client->kernel = init_kernel(m_man, sc_no, new_arch);
    }
    
    char kernel_path[256];
    sprintf (kernel_path, "%s/%s/kernel/", m_man->root, current_client->id);
    
    return OK;
}

// Get the kernel struct for the respective ip (create if necessary)
response_t _GETKERNEL (struct manager* m_man, char* ip, int sockfd, char** args, int n) {
    
}