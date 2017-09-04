#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  autogentoo.py
#  
#  Copyright 2017 Andrei Tumbar <atuser@Hyperion>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  

import socketrequest
import string
import sys

class Server:
    clients = []
    ids = []
    ip = ""
    port = 9490
    active = -1
    
    def __init__ (self, ip):
        self.ip = ip
    
    def regen (self):
        server_socket = socketrequest.SocketRequest (self.ip, self.port)
        reply = server_socket.send (b"SRV GETCLIENTS HTTP/1.0\n").decode ('utf-8')
        server_socket.close ()
        split = reply.split ("\n")
        self.clients = []
        self.ids = []
        for i in range (0, int(split[0])):
            self.ids.append (split[i+1])
            self.clients.append (Client (split[i+1], self))
        
        for x in self.clients:
            x.GETCLIENT ()
        
        self.get_active ()
    
    def get_active (self):
        active_socket = socketrequest.SocketRequest (self.ip, self.port)
        self.active = int (self.ids.index (active_socket.send (b"SRV GETACTIVE HTTP/1.0").decode('utf-8').split('\n')[0]))

class Client:
    _id = ""
    hostname = ""
    profile = ""
    CFLAGS = ""
    CXXFLAGS = ""
    CHOST = ""
    USE = ""
    EXTRAS = []
    server = None
    
    def __init__ (self, _id, server):
        self._id = _id
        self.server = server
    
    def GETCLIENT (self):
        temp_socket = socketrequest.SocketRequest (self.server.ip, self.server.port)
        data = temp_socket.send (("SRV GETCLIENT HTTP/1.0\n%s\n" % self._id).encode('utf-8')).decode ('utf-8')
        b_arr = data.split ("\n")
        sys.stdout.flush()
        extra_c = int (b_arr[1])
        
        self.CFLAGS = b_arr[2]
        self.CXXFLAGS = b_arr[3]
        self.CHOST = b_arr[4]
        self.USE = b_arr[5]
        self.EXTRAS = []
        
        self.hostname = b_arr[6];
        self.profile = b_arr[7];
        for k in range (0, extra_c):
            self.EXTRAS.append( b_arr[k+8]);

