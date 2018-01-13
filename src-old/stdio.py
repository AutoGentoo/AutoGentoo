#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  stdio.py
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


def printf (string, *_format):
    from sys import stdout
    stdout.write (string % _format)
    stdout.flush ()

def sprintf (string, *_format):
    return string % _format

def group(seq, sep):
    g = []
    for el in seq:
        if el == sep:
            yield g
            g = []
            continue
        g.append(el)
    yield g
