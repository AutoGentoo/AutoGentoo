/*
 * srv_handle.h
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

#ifndef __AUTOGENTOO_SRV_HANDLE_H__
#define __AUTOGENTOO_SRV_HANDLE_H__

#include <request.h>
#include <serve_client.h>

typedef enum {
    CREATE, // Create new serve_client
    INIT, // Initialize the new serve_client
    ACTIVATE, // Active ip to id
    GETCLIENT, // Get client information (CFLAGS, CHOST etc.)
    STAGE1, // Emerge system base packages
    EDIT, // Edit your make.conf and serve_client configuration
    GETCLIENTS, // Not to be confused with GETCLIENTS, first return line will be client_c followed by \n delimed ids
    GETACTIVE, // Returns id of active client from the ip of request
    GETSPEC, // Returns specs about the build server
    SYNC, // Run emerge --sync
    SCREMOVE, // Id to remove
    MNTCHROOT, // Mount chroot directories and start the thread
    DEVCREATE, // Specify ID on creation, if ID exists edit that client
    HKBUILD, // HEAD Kernel binary (file metadata)
    GKBUILD, // GET Kernel binary (binary file)
    GETKERNEL // Kernel struct metadata
} serve_c;

struct link_srv {
    serve_c command;
    int argc;
    char* ID;
};

extern struct link_srv link_methods [];

#define L_CREATE (struct link_srv) {CREATE, 5, "CREATE"}
#define L_INIT (struct link_srv) {INIT, 0, "INIT"}
#define L_ACTIVATE (struct link_srv) {ACTIVATE, 1, "ACTIVATE"} // ID of serve_client
#define L_GETCLIENT (struct link_srv) {GETCLIENT, 1, "GETCLIENT"} // Ask for id
#define L_STAGE1 (struct link_srv) {STAGE1, 0, "STAGE1"}
#define L_EDIT (struct link_srv) {EDIT, 6, "EDIT"}
#define L_GETCLIENTS (struct link_srv) {GETCLIENTS, 0, "GETCLIENTS"}
#define L_GETACTIVE (struct link_srv) {GETACTIVE, 0, "GETACTIVE"}
#define L_GETSPEC (struct link_srv) {GETSPEC, 0, "GETSPEC"}
#define L_SYNC (struct link_srv) {SYNC, 0, "SYNC"}
#define L_SCREMOVE (struct link_srv) {SCREMOVE, 1, "SCREMOVE"}
#define L_MNTCHROOT (struct link_srv) {MNTCHROOT, 0, "MNTCHROOT"}
#define L_DEVCREATE (struct link_srv) {DEVCREATE, 6, "DEVCREATE"}
#define L_HKBUILD (struct link_srv) {HKBUILD, 0, "HKBUILD"}
#define L_GKBUILD (struct link_srv) {GKBUILD, 0, "GKBUILD"}
#define L_GETKERNEL (struct link_srv) {GETKERNEL, 0, "GETKERNEL"}

struct link_srv get_link_srv (serve_c);
struct link_srv get_link_str (char*);

struct srv_f_link {
    request_t type;
    response_t (*func) (struct manager* m_man, char* ip, int sockfd, char** args, int n);
};

// Returns sent
response_t srv_handle (int sockfd, request_t type, struct manager* m_man, char** args, int n);

response_t _CREATE (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _ACTIVATE (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _INIT (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _GETCLIENT (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _STAGE1 (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _SYNC (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _EDIT (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _GETCLIENTS (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _GETACTIVE (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _GETSPEC (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _SCREMOVE (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _MNTCHROOT (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _DEVCREATE (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _HKBUILD (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _GKBUILD (struct manager* m_man, char* ip, int sockfd, char** args, int n);
response_t _GETKERNEL (struct manager* m_man, char* ip, int sockfd, char** args, int n);

extern struct srv_f_link srv_methods[];

#endif