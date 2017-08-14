/*
 * server.h
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

#ifndef __AUTOGENTOO_HTTP_SERVER__
#define __AUTOGENTOO_HTTP_SERVER__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <request.h>
#include <serve_client.h>


#define CONNMAX 1000
#define BYTES 102400
#define BUFSIZE 1024

extern char *ROOT;
extern char* S_PORT;
int  listenfd, clients[CONNMAX];
struct serve_client_manager *main_manager;

void error         (char *);
void server_start  (char *);
void server_respond(int);
void handle_exit   (int a);
void strrev        (char *p);
void send_request  (char *request, char* res);

#endif
