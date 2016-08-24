#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  flags.py
#  
#  Copyright 2016 Unknown <atuser@Kronos>
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


"""
    cli? ( ^^ ( readline libedit ) )\
    truetype? ( gd ) \
    vpx? ( gd ) \ 
    cjk? ( gd ) \ 
    exif? ( gd ) \
    xpm? ( gd ) \
    gd? ( zlib ) \
    simplexml? ( xml ) \
    soap? ( xml ) \
    wddx? ( xml ) \
    xmlrpc? ( || ( xml iconv ) ) \
    xmlreader? ( xml ) \
    xslt? ( xml ) \
    ldap-sasl? ( ldap ) \
    mhash? ( hash ) \
    phar? ( hash ) \
    libmysqlclient? ( || ( mysql mysqli pdo ) ) \
    qdbm? ( !gdbm ) \
    readline? ( !libedit ) \
    recode? ( !imap !mysql !mysqli ) \
    sharedmem? ( !threads ) \
    !cli? ( !cgi? ( !fpm? ( !apache2? ( !embed? ( cli ) ) ) ) )
"""

class Flags:
    """
    blocks {
    
    """
    
    blocks = {}
    

def getClose (paren):
    curr_p = 0
    curr_x = 0
    
    for x in paren:
        if (x == "("):
            curr_p += 1
        if (x == ")"):
            curr_p -= 1
        if (curr_p == 0):
            break
        curr_x += 1
    return curr_x

def TOPBLOCK (topBlock):
    firstword = topBlock[0:topBlock.find (" ")]
    
    block = topBlock[len(firstword) + 3: ]
    return block

def main(args):
    print(TOPBLOCK ("cli? ( ^^ ( readline libedit ) ) truetype? ( gd ) vpx? ( gd ) cjk? ( gd ) exif? ( gd ) xpm? ( gd ) gd? ( zlib ) simplexml? ( xml ) soap? ( xml ) wddx? ( xml ) xmlrpc? ( || ( xml iconv ) ) xmlreader? ( xml ) xslt? ( xml ) ldap-sasl? ( ldap ) mhash? ( hash ) phar? ( hash ) libmysqlclient? ( || ( mysql mysqli pdo ) ) qdbm? ( !gdbm ) readline? ( !libedit ) recode? ( !imap !mysql !mysqli ) sharedmem? ( !threads ) !cli? ( !cgi? ( !fpm? ( !apache2? ( !embed? ( cli ) ) ) ) )"))
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main(sys.argv))
