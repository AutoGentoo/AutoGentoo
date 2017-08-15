/*
 * response.h
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


#ifndef __AUTOGENTOO_HTTP_CODES__
#define __AUTOGENTOO_HTTP_CODES__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef enum {
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_NO_CONTENT = 204,
    HTTP_BAD_REQUEST = 400,
    HTTP_UNAUTHORIZED = 401,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_METHOD_NOT_ALLOWED = 405,
    HTTP_REQUEST_TIMEOUT = 408,
    HTTP_INTERNAL_ERROR = 500,
    HTTP_NOT_IMPLEMENTED = 501,
    HTTP_BAD_GATEWAY = 502,
    HTTP_SERVICE_UNAVAILABLE = 503
} response_nt;

typedef struct {
    response_nt code;
    char * message;
    size_t len;
} response_t;

extern response_t OK;
extern response_t CREATED;
extern response_t NO_CONTENT;
extern response_t BAD_REQUEST;
extern response_t UNAUTHORIZED;
extern response_t FORBIDDEN;
extern response_t NOT_FOUND;
extern response_t METHOD_NOT_ALLOWED;
extern response_t REQUEST_TIMEOUT;
extern response_t INTERNAL_ERROR;
extern response_t NOT_IMPLEMENTED;
extern response_t BAD_GATEWAY;
extern response_t SERVICE_UNAVAILABLE;

ssize_t rsend (int fd, response_t code);

#endif
