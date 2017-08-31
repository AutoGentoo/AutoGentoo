#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  socketrequest.py
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


import socket

class SocketRequest:
    socket = None
    ip = ""
    port = None
    request = "" # Only the last request, resets after next request
    
    def __init__ (self, ip, port):
        self.ip = ip
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect ((self.ip, self.port))
    
    def send (self, request):
        self.request = request
        self.socket.sendall (request)
        return self.socket.recv (1024)
        
    
    def close (self):
        self.socket.close()
