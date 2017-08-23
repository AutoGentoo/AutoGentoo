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

response_t m_install (char* command, struct manager * m_man, struct serve_client client) {
    char cmd[320];
    char opts[256];
    get_emerge_command (m_man, client, opts);
    sprintf (cmd, "%s %s", opts, command);
    printf ("%s\n", cmd);
    fflush (stdout);
    if (system (cmd) != 0)
        return INTERNAL_ERROR;
    return OK;
}

response_t m_remove (char* command, struct manager * m_man, struct serve_client client) {
    char cmd[320];
    char opts[256];
    get_emerge_command (m_man, client, opts);
    sprintf (cmd, "%s --unmerge %s", opts, command);
    printf ("%s\n", cmd);
    fflush (stdout);
    if (system (cmd) != 0)
        return INTERNAL_ERROR;
    return OK;
}

response_t exec_method_client (request_t type, char * command) {
    struct _client client;
    char *client_config = "/etc/portage/autogentoo.conf";
    if (access (client_config, F_OK) == -1)
        _mkdir ("/etc/portage");
        
        int fd = open (client_config, O_RDWR);
        
}

response_t exec_method (request_t type, struct manager * man, char* command, int sockfd) {
    int i;
    for (i=0; i != sizeof (methods) / sizeof (struct method_s); i++) {
        if (methods[i].type == type) {
            char * ip = get_ip_from_fd (sockfd);
            int client_no = get_client_from_ip (man, ip);
            if (client_no < 0) {
                return UNAUTHORIZED;
            }
            printf ("%s\n", man->clients[client_no].hostname);
            return methods[i].method (command, man, man->clients[client_no]);
        }
    }
    // Method could not be found
    return NOT_IMPLEMENTED;
}

response_t ask_server (char* ip, struct client_request req, char *message) {
    int sockfd, n;
    struct sockaddr_in server;
    
    char buffer[256];
    
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
    
    sprintf (buffer, "CMD %d %s", (int)req.type, req.atom);
    
    write (sockfd, buffer, strlen(buffer));
    
    response_nt res_t;
    char _m[256];
    recv (sockfd, _m, sizeof (_m), 0);
    
    strtok (_m, " ");
    sscanf (strtok (NULL, " "), "%d", &res_t);
    strcpy (message, _m);
    close (sockfd);
    return get_res (res_t);
}
