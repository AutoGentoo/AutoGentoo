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


#define OK (response_t) {HTTP_OK, "OK", 2}
#define CREATED (response_t) {HTTP_CREATED, "Created", 7}
#define NO_CONTENT (response_t) {HTTP_NO_CONTENT, "No Content", 10}
#define BAD_REQUEST (response_t) {HTTP_BAD_REQUEST, "Bad Request", 11}
#define UNAUTHORIZED (response_t) {HTTP_UNAUTHORIZED, "Unauthorized", 12}
#define FORBIDDEN (response_t) {HTTP_FORBIDDEN, "Forbidden", 9}
#define NOT_FOUND (response_t) {HTTP_NOT_FOUND, "Not found", 9}
#define METHOD_NOT_ALLOWED (response_t) {HTTP_METHOD_NOT_ALLOWED, "Method Not Allowed", 18}
#define REQUEST_TIMEOUT (response_t) {HTTP_REQUEST_TIMEOUT, "Requeset Timeout", 16}
#define INTERNAL_ERROR (response_t) {HTTP_INTERNAL_ERROR, "Internal Error", 14}
#define NOT_IMPLEMENTED (response_t) {HTTP_NOT_IMPLEMENTED, "Method Not Implemented", 22}
#define BAD_GATEWAY (response_t) {HTTP_BAD_GATEWAY, "Bad Gateway", 11}
#define SERVICE_UNAVAILABLE (response_t) {HTTP_SERVICE_UNAVAILABLE, "Service Unavailable", 19}

extern response_t res_list[];

ssize_t rsend (int fd, response_t code);
response_t get_res (response_nt x);

#endif
