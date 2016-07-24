#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  http-serve.py
#  
#  Copyright 2016 Andrei Tumbar <atuser@Kronos-Ubuntu>
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


import http.server
import socketserver
import sys

def main (argv):
  PORT = 8000
  ip = "localhost"
  if (len(argv)):
    ip = argv[1]
  
  Handler = http.server.SimpleHTTPRequestHandler
  
  httpd = socketserver.TCPServer((ip, PORT), Handler)
  
  print("Started HTTP Server on port", PORT)
  httpd.serve_forever()
  
  return 0;

if __name__ == "__main__":
  main(sys.argv)
