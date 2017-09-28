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

extern struct srv_f_link srv_methods[];

#endif