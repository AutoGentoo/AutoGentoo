/*
 * response.c
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


#include <response.h>
#include <stdio.h>
#include <unistd.h>

ssize_t rsend (int fd, response_t code) {
    char message[40];
    sprintf (message, "HTTP/1.0 %d %s\n", code.code, code.message);
    return write(fd, message, 14 + code.len);
}

response_t res_list[] = {
    OK,
    CREATED,
    NO_CONTENT,
    BAD_REQUEST,
    UNAUTHORIZED,
    FORBIDDEN,
    NOT_FOUND,
    METHOD_NOT_ALLOWED,
    REQUEST_TIMEOUT,
    INTERNAL_ERROR,
    NOT_IMPLEMENTED,
    BAD_GATEWAY,
    SERVICE_UNAVAILABLE
};

response_t get_res (response_nt x) {
    int i;
    for (i=0;i != sizeof (res_list) / sizeof (res_list[0]); i++) {
        if (res_list[i].code == x) {
            return res_list[i];
        }
    }
}
