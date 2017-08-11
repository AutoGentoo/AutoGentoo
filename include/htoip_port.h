/*
 * htoip_port.h
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


#ifndef __AUTOGENTOO_IP_HOSTNAME_PORT__
#define __AUTOGENTOO_IP_HOSTNAME_PORT__

#include <stdio.h>
#include <Python.h>
#include <string.h> //memset
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h> //hostent
#include <arpa/inet.h>

int hostname_to_ip (char * hostname, char * ip);
static PyObject * hostnameToIp (PyObject * self, PyObject * args);

#endif
