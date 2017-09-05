#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  portage.py
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

from collections import namedtuple
import os
from stdio import *

current_keywords = ["amd64", "x86", "alpha", "arm", "hppa", "ia64", "ppc", "ppc64", "sparc"]


Keyword = namedtuple ("Keyword", ('identifier', 'stable'))
Version = namedtuple ("Version", ('id', 'ebuild', 'keywords', 'slot'))
Flag = namedtuple ("Flag", ("name", "description"))
Package = namedtuple ("Package", ("name", "description", "homepage", "globalflags", "localflags", "versions"))

class portage:
    packages = {} # category: [packages]
    global_use = []
    path = ""
    
    def __init__ (self, path="/usr/portage"):
        self.path = os.path.expanduser(path)
        self.generate_global_use ()
        self.generate_packages ()
    
    def readlines (self, path):
        out = []
        with open ("%s/%s" % (self.path, path), "r") as f:
            out = f.readlines ()
            f.close ()
        return out
    
    def generate_global_use (self):
        b = self.readlines ("profiles/use.desc")
        for x in b:
            x = x[:-1] # Remove newline
            if not x: # Skip comments and empty lines
                continue
            elif x[0] == "#":
                continue
            i_b = x.find (" - ")
            self.global_use.append (Flag (x[:i_b], x[i_b+3:]))
    
    def get_dirs (self, path):
        return [name for name in os.listdir(path)
                if os.path.isdir(os.path.join(path, name))]
    
    def parse_version_id (self, buff):
        if (buff[buff.rfind("-")+1] == 'r'): # Revision number
            return buff[:buff.rfind ("-", 0, buff.rfind ("-"))]
        else:
            return buff[:buff.rfind ("-")]
    
    def parse_keywords (self, buf):
        stable = True
        if (buf == ""):
            return None
        if (buf[0] == "~"):
            stable = False
            buf = buf[1:]
        if (buf not in current_keywords):
            return None
        return Keyword (buf, stable)
    
    def get_full_keywords (self, buf):
        added = []
        print (buf)
        for x in buf:
            if (x == ""):
                continue
            if (x[0] == "~"):
                x = x[1:]
            if x not in current_keywords:
                continue
            added.append (x)
        return set(current_keywords) - set(added)

    def parse_ebuild (self, pkg, cat, ebuild):
        lines = self.readlines ("%s/%s/%s" % (pkg, cat, ebuild))
        parsed = {}
        for line in lines:
            __split = line.split("=")
            if len(__split) != 2:
                continue
            parsed[__split[0]] = __split[1].replace ("\"", "")
        
        try:
            v_architectures = parsed["KEYWORDS"].strip().split (" ")
        except KeyError:
            v_architectures = current_keywords
        v_architectures = self.get_full_keywords (v_architectures) | set(v_architectures)
        v_keywords = list(self.parse_keywords (x) for x in v_architectures if self.parse_keywords (x) != None)
        v_ebuild = ebuild
        print (v_keywords)
        try:
            v_slot = parsed["SLOT"]
        except KeyError:
            v_slot = "0"
        v_id = self.parse_version_id (ebuild.replace (".ebuild", ""))
        
    
    def generate_package (self, cat, pkg):
        ebuilds = []
        pkg_dir = "%s/%s/" % (cat, pkg)
        manifest = self.readlines ("%s/Manifest" % pkg_dir)
        for line in manifest:
            tokens = line.split (' ')
            if tokens[0] != "EBUILD":
                continue
            ebuilds.append (tokens[1])
        
        versions = []
        for ebuild in ebuilds:
            self.parse_ebuild (cat, pkg, ebuild)
    
    def generate_packages (self):
        dirs = set(self.get_dirs(self.path))
        for cat in dirs - set(("distfile", "eclass", "licenses", "metadata", "profiles", "scripts", "packages")):
            self.packages [cat] = []
            cat_pkgs = self.get_dirs ("%s/%s" % (self.path, cat))
            for pkg in cat_pkgs:
                self.packages[cat].append (self.generate_package(cat,pkg))
        
def main(args):
    temp = portage ("~/Downloads/portage")
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main(sys.argv))
