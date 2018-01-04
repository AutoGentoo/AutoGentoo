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
    hosts = []
    ids = []
    ip = ""
    port = 9491
    active = -1
    
    def __init__ (self, ip):
        self.ip = ip
    
    def regen (self):
        server_socket = socketrequest.SocketRequest (socketrequest.Address(self.ip, self.port))
        reply = server_socket.send (b"SRV GETHOSTS HTTP/1.0\n").decode ('utf-8')
        server_socket.close ()
        split = reply.split ("\n")
        self.hosts = []
        self.ids = []
        for i in range (0, int(split[0])):
            self.ids.append (split[i+1])
            print (split[i+1])
            self.hosts.append (Host (split[i+1], self))
        
        for x in self.hosts:
            x.GETHOST ()
        
        self.get_active ()
    
    def array_find (self, arr, el):
        for i, x in enumerate (arr):
            if x == el:
                print (True)
                return i
            print (False)
        return -1
    
    def get_active (self):
        active_socket = socketrequest.SocketRequest (socketrequest.Address(self.ip, self.port))
        
        active_id = active_socket.send (b"SRV GETACTIVE").decode('utf-8')
        active_socket.close ()
        active_id = str(active_id[:active_id.find ("\n")])
        self.active = self.array_find (self.ids, active_id)
        print (self.active)
        
    def create (self, hostname, profile, chost, cflags, use, extra=""):
        create_socket = socketrequest.SocketRequest (socketrequest.Address(self.ip, self.port))
        request = ("SRV CREATE %s\n%s\n%s\n%s\n%s\n%s\n%s" % (
            len(extra.split ('\n')) - 1 if len(extra.split ('\n')) > 1 else "HTTP/1.0",
            hostname,
            profile,
            chost,
            cflags,
            use,
            "%s\n" % extra if extra != "" else ""
        ))
        new_id = create_socket.send (request.encode ("utf-8")).decode ("utf-8").split ("\n")[0]
        
        # No need to regen
        self.ids.append (new_id)
        self.hosts.append (Host (new_id, self))
        self.hosts[-1].GETHOST ()
        self.get_active ()
        
        return new_id

class Host:
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
    
    def GETHOST (self):
        temp_socket = socketrequest.SocketRequest (socketrequest.Address(self.server.ip, self.server.port))
        data = temp_socket.send (("SRV GETHOST %s\n" % self._id).encode('utf-8')).decode ('utf-8')
        b_arr = data.split ("\n")
        sys.stdout.flush()
        extra_c = int (b_arr[0])
        
        self.CFLAGS = b_arr[1]
        self.CXXFLAGS = b_arr[2]
        self.CHOST = b_arr[3]
        self.USE = b_arr[4]
        self.hostname = b_arr[5];
        self.profile = b_arr[6];
        self.EXTRAS = []
        
        for k in range (0, extra_c):
            self.EXTRAS.append( b_arr[k+7]);

