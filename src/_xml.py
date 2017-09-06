#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  xml.py
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


import xml.etree.ElementTree as ET

class XML_EL(object):
    def __init__(self, elem):
        self.etElem = elem

    def __getitem__(self, name):
        res = self._getattr(name)
        if res is None:
            raise AttributeError( "No attribute named '%s'" % name)
        return res

    def __getattr__(self, name):
        res = self._getelem(name)
        if res is None:
            raise IndexError("No element named '%s'" % name)
        return res

    def _getelem(self, name):
        res = self.etElem.findall(name)
        if res is None:
            return None
        if (len(res) == 1):
            return XML_EL(res[0])
        out = []
        for x in res:
            out.append (XML_EL(x))
        return out

    def _getattr(self, name):
        return self.etElem.get(name)
        
    def find_attr (self, name, attr_name, attr_val):
        res_buf = self._getelem (name)
        for x in res_buf:
            try:
                x[attr_name]
            except IndexError:
                continue
            else:
                if x[attr_name] == attr_val:
                    return x
    
    def text (self):
        return self.etElem.text

class _XML(object):
    "Wrapper around an ElementTree."
    def __init__(self, fname):
        self.doc = ET.parse(fname)

    def __getattr__(self, name):
        if self.doc.getroot().tag != name:
            raise IndexError( "No element named '%s'" % name)
        return XML_EL(self.doc.getroot())

    def getroot(self):
        return self.doc.getroot()
