/*
 * packageConfig.cxx
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos>
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

#ifndef __AUTOGENTOO_PACKAGE_CONFIG_FILE__
#define __AUTOGENTOO_PACKAGE_CONFIG_FILE__

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../package/package.hpp"
#include "../package/EmergePackage.hpp"
#include "../tools/_misc_tools.hpp"

using namespace std;

void PackageConfig ( EmergePackage pkg, string name )
{
	ofstream file;
	file.open ( name.c_str ( ), ios::app  );
	
	string lineOne ( "[" + pkg.path + "]" + "\n" );
	file << lineOne;
	
	string _version ( "version=\"" + misc::merge <string> ( pkg.release.v_str, "." ) );
	if ( pkg.release.has_revision )
	{
		_version += "-" + pkg.release.revision;
	}
	_version += "\"\n";
	file << _version;
	
	string _name ( "name=\"" + pkg.name + "\"\n" );
	file << _name;
	
	string _cat ( "catagory=\"" + pkg.catagory + "\"\n" );
	file << _cat;
	
	string _slot ( "slot=\"" + pkg.slot + "\"\n" );
	file << _slot;
	cout << "--------------------------------" << endl;
	cout << pkg.properties.attrVec.size ( ) << endl;
	vector < string > keys;
	for ( size_t i = 0; i != pkg.properties.attrVec.size ( ); i++ )
	{
		string z = pkg.properties.attrVec [ i ];
		cout << z << endl;
		pkg.properties.init ( );
		if ( pkg.properties.attrMap[ z ] )
		{
			cout << z << endl;
			keys.push_back ( z );
		}
	}
	string _keys ( "keys=[" + misc::merge <string > ( keys, "," ) + "]\n" );
	file << _keys;
	
	string _file ( "file=\"" + pkg.file + "\"\n" );
	file << _file;
	
	if ( pkg.old._contructed )
	{
		string _version_old ( "old=\"" + misc::merge <string> ( pkg.old.v_str, "." ) );
		if ( pkg.old.has_revision )
		{
			_version_old += "-" + pkg.old.revision;
		}
		if ( _version_old != "old=\"" )
		{
			_version_old += "\"\n";
			file << _version_old;
		}
	}
}
#endif
