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

response_t m_install_s (char* command, int sockfd) {
    char * ip = get_ip_from_fd (sockfd);
    //struct serve_client * client = get_client_from_ip (ip)
    //struct emerge_session es = init_emerge_session (sockfd, 
    
    return OK;
}

response_t m_remove_s (char* command, int sockfd) {
    return NOT_IMPLEMENTED;
}

response_t m_install_c (char* command, int sockfd) {
    return NOT_IMPLEMENTED;
}
response_t m_remove_c (char* command, int sockfd) {
    return NOT_IMPLEMENTED;
}
response_t m_get (char* command, int sockfd) {
    return NOT_IMPLEMENTED;
}
response_t m_install (char* command, int sockfd) {
    return NOT_IMPLEMENTED;
}
response_t m__remove (char* command, int sockfd) {
    return NOT_IMPLEMENTED;
}

response_t exec_method (request_t type, char* command, int sockfd) {
    int i;
    for (i=0; i != sizeof (methods) / sizeof (struct method_s); i++) {
        if (methods[i].type == type) {
            return methods[i].method (command, sockfd);
        }
    }
    // Method could not be found
    return NOT_IMPLEMENTED;
}
