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

struct response_t OK = {HTTP_OK, "OK", 2};
struct response_t CREATED = {HTTP_CREATED, "Created", 7};
struct response_t NO_CONTENT = {HTTP_NO_CONTENT, "No Content", 10};
struct response_t BAD_REQUEST = {HTTP_BAD_REQUEST, "Bad Request", 11};
struct response_t UNAUTHORIZED = {HTTP_UNAUTHORIZED, "Unauthorized", 12};
struct response_t FORBIDDEN = {HTTP_FORBIDDEN, "Forbidden", 9};
struct response_t NOT_FOUND = {HTTP_NOT_FOUND, "Not found", 9};
struct response_t METHOD_NOT_ALLOWED = {HTTP_METHOD_NOT_ALLOWED, "Method Not Allowed", 18};
struct response_t REQUEST_TIMEOUT = {HTTP_REQUEST_TIMEOUT, "Requeset Timeout", 16};
struct response_t INTERNAL_ERROR = {HTTP_INTERNAL_ERROR, "Internal Error", 14};
struct response_t NOT_IMPLEMENTED = {HTTP_NOT_IMPLEMENTED, "Method Not Implemented", 22};
struct response_t BAD_GATEWAY = {HTTP_BAD_GATEWAY, "Bad Gateway", 11};
struct response_t SERVICE_UNAVAILABLE = {HTTP_SERVICE_UNAVAILABLE, "Service Unavailable", 19};

ssize_t rsend (int fd, struct response_t code) {
    char message[40];
    sprintf (message, "HTTP/1.0 %d %s\n", code.code, code.message);
    return send(fd, message, 14 + code.len, 0);
}
