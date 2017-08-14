/*
 * request.h
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


#ifndef __AUTOGENTOO_CLIENT_EMERGE__
#define __AUTOGENTOO_CLIENT_EMERGE__

#include <stdio.h>

typedef enum {
    install_s,  // Install only on the server (used for build tools)
    remove_s,   // Remove package from server
    install_c,  // Install on the client machine only
    remove_c,   // Remove from the client
    get,        // Download the binary, dont install
    install,    // Install on both client and server
    _remove      // Remove from both client and server
} request_t;

struct client_request {
    char * atom;
    request_t type;
};

void process_request (struct client_request t);

#endif
