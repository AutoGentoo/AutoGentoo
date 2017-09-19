/*
 * request.c
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


#include <request.h>
#include <chroot.h>
#include <_string.h>
#include <unistd.h>

char *request_names[] = {
    "INSTALL_S",
    "REMOVE",
    "INSTALL",
    "REMOVE_C"
};

struct method_s methods [] = {
    INSTALL_S,
    REMOVE,
    INSTALL,
    REMOVE_C,
};

response_t __m_install (char* command, struct manager * m_man, int sc_no, char* ip, int fd) {
    char cmd[2048];
    char opts[1024];
    emerges (m_man, sc_no, opts);
    sprintf (cmd, "%s %s", opts, command);
    printf ("%s\n", cmd);
    if (system (cmd) != 0) {
        return INTERNAL_ERROR;
    }
    
    return OK;
}

response_t m_install (char* command, struct manager * m_man, int sc_no, char* ip, int fd) {
    char root[256];
    sprintf (root, "%s/%s/", m_man->root, m_man->clients[sc_no].id);
    
    strcpy (root, path_normalize (root));
    
    char cmd[2048];
    printf ("%s /usr/bin/emerge %s", root, command);
    
    char *args[] = {
        "chroot",
        cmd,
        NULL
    };
    
    printf ("/bin/chroot %s /usr/bin/emerge %s\n", root, command);
    
    pid_t install_pid = fork ();
    if (install_pid == 0) {
        execve ("/bin/chroot", args, NULL);
        exit (-1);
    }
    
    int install_ret;
    waitpid (install_pid, &install_ret, 0); // Wait until finished
    
    if (install_ret != 0) {
        return INTERNAL_ERROR;
    }
    return OK;
}

response_t m_remove (char* command, struct manager * m_man, int sc_no, char* ip, int fd) {
    char cmd[2048];
    char opts[1024];
    emergec (opts);
    sprintf (cmd, "%s --unmerge %s", opts, command);
    printf ("%s\n", cmd);
    fflush (stdout);
    if (system (cmd) != 0)
        return INTERNAL_ERROR;
    return OK;
}

response_t exec_method_client (request_t type, char * command) {
    char cmd[2048];
    char opts[1024];
    emergec (opts);
    if (type == install) {
        sprintf (cmd, "%s %s", opts, command);
    }
    else {
        sprintf (cmd, "%s --unmerge %s", opts, command);
    }
    printf ("%s\n", cmd);
    fflush(stdout);
    if (system(cmd) !=0)
        return INTERNAL_ERROR;
    return OK;
}

response_t exec_method (char *type, struct manager * man, char* command, char *ip, int fd) {
    int i;
    for (i=0; i != sizeof (methods) / sizeof (struct method_s); i++) {
        if (strcmp(request_names[i], type) == 0) {
            int client_no = get_client_from_ip (man, ip);
            if (client_no < 0) {
                return UNAUTHORIZED;
            }
            return methods[i].method (command, man, client_no, ip, fd);
        }
    }
    // Method could not be found
    return NOT_IMPLEMENTED;
}

void serve_req(char* ip, char* req, char *message) {
    int sockfd, n;
    struct sockaddr_in server;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) {
        perror ("ERROR opening socket");
    }
    
    server.sin_addr.s_addr = inet_addr (ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(9490);
    
    if (connect (sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf ("connect() failed: %d\n",errno);
        fflush(stdout);
        exit(1);
    }
    
    write (sockfd, req, strlen(req));
    
    char _m[256];
    recv (sockfd, _m, sizeof (_m), 0);
    
    strcpy (message, _m);
    close (sockfd);
}

response_t ask_server (char* ip, struct client_request req, char *message) {
    char buffer[256];
    char _m[256];
    sprintf (buffer, "CMD %d %s", (int)req.type, req.atom);
    
    serve_req (ip, buffer, &_m[0]);
    
    response_nt res_t;
    
    strtok (_m, " ");
    sscanf (strtok (NULL, " "), "%d", (int*)&res_t);
    strcpy (message, _m);
    return get_res (res_t);
}
