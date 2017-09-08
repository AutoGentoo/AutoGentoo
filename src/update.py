#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  update.py
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


from os import system
import sys

def main(args):
    buf = sys.stdin.readlines()
    pkgs = []
    for x in buf:
        if x[0] != "[":
            continue
        i = x.find(" ")
        if (x[1:i] != "ebuild"):
            continue
        pkgbuf = x[x.find("]") + 2:]
        pkgs.append ("=%s" % pkgbuf[:pkgbuf.find(" ")])
    
    print ("emerge -q --update --deep %s" % " ".join (pkgs))
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main(sys.argv))
