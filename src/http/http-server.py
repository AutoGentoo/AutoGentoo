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
import sys, os, posixpath, urllib

# modify this to add additional routes
global ROUTES
global ip
global PORT
global basedir

PORT = 8000
ip = "localhost"
basedir = ''
if (len(sys.argv) > 1):
  ip = sys.argv[1]
  if (len(sys.argv) > 2):
    basedir = sys.argv[2]

ROUTES = (
  ['', basedir],
)

class RequestHandler(http.server.SimpleHTTPRequestHandler):
  
  def translate_path(self, path):
    """translate path given routes"""

    # set default root to cwd
    root = os.getcwd()
    
    # look up routes and set root directory accordingly
    for pattern, rootdir in ROUTES:
      if path.startswith(pattern):
        # found match!
        path = path[len(pattern):]  # consume path up to pattern len
        root = rootdir
        break
    
    # normalize path and prepend root directory
    path = path.split('?',1)[0]
    path = path.split('#',1)[0]
    path = posixpath.normpath(urllib.parse.unquote(path))
    words = path.split('/')
    words = filter(None, words)
    
    path = root
    for word in words:
      drive, word = os.path.splitdrive(word)
      head, word = os.path.split(word)
      if word in (os.curdir, os.pardir):
        continue
      path = os.path.join(path, word)

    return path

def main ():
  Handler = RequestHandler
  
  httpd = socketserver.TCPServer((ip, PORT), Handler)
  
  print("Started HTTP Server on port", PORT)
  try:
    httpd.serve_forever()
  except KeyboardInterrupt:
    print ("Stopping...")
    return 0
  
  return 0

main ()
