#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  http-client.py
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

import http.client
import urllib
import os

def sync_dir (url, dir):
    out = []
    os.system("wget -q %s/%s -O sync-%s.temp" % (url, dir, dir[:-1]))
    synctemp = open("sync-%s.temp" % dir[:-1], "r").readlines ()
    os.remove ("sync-%s.temp" % dir[:-1])
    
    for file in synctemp:
        out.append (file)
    return out

def sync (url):
    out = {}
    top = []
    sys.stdout.write ("Downloading file list...")
    os.system("wget -q %s -O sync.temp" % url)
    sys.stdout.write ("done\n")
    synctemp = open("sync.temp", "r").readlines ()
    os.remove("sync.temp")
    
    for line in synctemp:
        if not line.startswith ("<li>"):
            continue
        top.append (line[line.find('"')+1:line.rfind('"')]);
    for _dir in top:
        if _dir.endswith ("/"):
            out[_dir[:-1]] = sync_dir (url, _dir)
    
    return out

def main(args):
    IP = ""
    
    if (not len(args) > 1):
        print ("You must enter a IP address")
        IP = input ("IP: ")
    else:
        IP = args[1]
    
    print (sync("http://%s:8000/" % IP))
    
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main(sys.argv))
