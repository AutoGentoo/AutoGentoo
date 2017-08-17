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

struct method_s methods [] = {
    INSTALL_S,
    REMOVE_S,
    INSTALL_C,
    REMOVE_C,
    GET,
    INSTALL,
    _REMOVE
};

response_t m_install_s (char* command, struct manager * m_man, struct serve_client client) {
    char cmd[288];
    char opts[256];
    get_emerge_command (m_man, client, opts);
    sprintf (cmd, "%s %s", opts, command);
    printf ("%s\n", cmd);
    fflush (stdout);
    if (system (cmd) != 0)
        return INTERNAL_ERROR;
    return OK;
}

response_t m_remove_s (char* command, struct manager * m_man, struct serve_client client) {
    return NOT_IMPLEMENTED;
}

response_t m_install_c (char* command, struct manager * m_man, struct serve_client client) {
    return NOT_IMPLEMENTED;
}
response_t m_remove_c (char* command, struct manager * m_man, struct serve_client client) {
    return NOT_IMPLEMENTED;
}
response_t m_get (char* command, struct manager * m_man, struct serve_client client) {
    return NOT_IMPLEMENTED;
}
response_t m_install (char* command, struct manager * m_man, struct serve_client client) {
    return NOT_IMPLEMENTED;
}
response_t m__remove (char* command, struct manager * m_man, struct serve_client client) {
    return NOT_IMPLEMENTED;
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
            return methods[i].method (command, man, man->clients[client_no]);
        }
    }
    // Method could not be found
    return NOT_IMPLEMENTED;
}
