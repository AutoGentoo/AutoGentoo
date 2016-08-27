/*
 * flags-bin.cpp
 * 
 * Copyright 2016 Unknown <atuser@Kronos>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <iostream>
#include <flags.hh>

int main(int argc, char **argv)
{
	parseblock ("cli? ( ^^ ( readline libedit ) ) truetype? ( gd ) vpx? ( gd ) cjk? ( gd ) exif? ( gd ) xpm? ( gd ) gd? ( zlib ) simplexml? ( xml ) soap? ( xml ) wddx? ( xml ) xmlrpc? ( || ( xml iconv ) ) xmlreader? ( xml ) xslt? ( xml ) ldap-sasl? ( ldap ) mhash? ( hash ) phar? ( hash ) libmysqlclient? ( || ( mysql mysqli pdo ) ) qdbm? ( !gdbm ) readline? ( !libedit ) recode? ( !imap !mysql !mysqli ) sharedmem? ( !threads ) !cli? ( !cgi? ( !fpm? ( !apache2? ( !embed? ( cli ) ) ) ) )");
	return 0;
}
