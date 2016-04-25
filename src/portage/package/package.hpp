/*
 * package.hpp
 * 
 * Copyright 2015 Andrei Tumbar <atuser@Kronos>
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

#ifndef __AUTOGENTOO_PACKAGE__
#define __AUTOGENTOO_PACKAGE__

#include <iostream>
#include <string>
#include "../tools/_misc_tools.hpp"
#include "version.hpp"

using namespace std;

class Package
{
	public:
	
	string packagestr;
	string name;
	string path;
	string catagory;
	string directory;
	string releaseStr;
	string file;
	string slot;
	version release;
	string fullPackage;
	
	Package ( string input = "" )
	{
		if ( !input.empty ( ) )
		{
			init ( input );
		}
	}
	
	void init ( string input_package )
	{
		/* Key
		 * + added by operator '+'
		 * - subtracted
		 * _ given that it is added by str.substr
		 * ^ found by a str.find() and length ()
		 * * Note added at bottom
		*/
		
		/*
		 * dev-lang/python-2.7.10-r1::gentoo
		 *                          --------
		*/
		size_t slotDivide = input_package.find ( "::" );
		slot = misc::substr ( input_package, slotDivide + 2, input_package.length ( ) );
		input_package.erase ( slotDivide, slot.length ( ) + 2 );
		fullPackage = input_package;
		
		/*
		 * dev-lang/python-2.7.10-r1
		 * ________^
		*/
		catagory = input_package.substr ( 0, input_package.find ( "/" ) );
		
		/*
		 * dev-lang/python-2.7.10-r1
		 *         ^_____________^
		*/
		name = misc::substr ( input_package, catagory.length ( ) + 1, misc::rfind ( input_package, '-' ) );
		/*
		 * dev-lang/python-2.7.10-r1
		 *         ^________________^
		*/
		string _file = misc::substr ( input_package, catagory.length ( ) + 1, input_package.length ( ) );
		int versionSplit;
		/* 
		 * dev-lang/python-2.7.10-r1
		 *                        ^
		*/
		
		if ( input_package.at ( misc::rfind ( input_package, '-' ) + 1 ) == 'r' )
		{
			/* Has revision
			* python-2.7.10-r1
			* ______^------*
			* Skipped because name = python-2.7.10
			*/
			versionSplit = misc::rfind ( name, '-' );
			name = misc::substr ( name, 0, misc::rfind ( name, '-' ) );
			directory = name.substr ( 0, versionSplit );
		}
		else
		{
			/* No revision
			* python-2.7.10
			*       ^
			*/
			versionSplit = _file.rfind ( "-" );
			directory = name;
		}
		
		path = catagory + "/" + directory;
		/*
		* python-2.7.10-r1
		* ------^_________^
		*/
		releaseStr = misc::substr ( _file, versionSplit + 1, file.length ( ) );
		release.init ( releaseStr );
		_file = name + "-" + release._in_str;
		
		slot = release.slot;
		
		file = "/usr/portage/" + catagory + "/" + directory + "/" + _file + ".ebuild";
	}
	
	void pkg_fetch ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " fetch" );
		system ( cmd.c_str ( ) );
	}
	void pkg_setup ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " setup" );
		system ( cmd.c_str ( ) );
	}
	void src_unpack ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " unpack" );
		system ( cmd.c_str ( ) );
	}
	void src_prepare ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " prepare" );
		system ( cmd.c_str ( ) );
	}
	void src_configure ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " configure" );
		system ( cmd.c_str ( ) );
	}
	void src_compile ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " compile" );
		system ( cmd.c_str ( ) );
	}
	void src_test ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " test" );
		system ( cmd.c_str ( ) );
	}
	void src_install ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " install" );
		system ( cmd.c_str ( ) );
	}
	void pkg_preinst ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " preinst" );
		system ( cmd.c_str ( ) );
	}
	void pkg_postinst ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " postinst" );
		system ( cmd.c_str ( ) );
	}
	void pkg_prerm ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " prerm" );
		system ( cmd.c_str ( ) );
	}
	void pkg_postrm ( string _file = "", string ebuild_opts = "" )
	{
		if ( _file.empty ( ) )
		{
			_file = file;
		}
		string cmd ( "ebuild " + ebuild_opts + " " +_file + " postrm" );
		system ( cmd.c_str ( ) );
	}
};
void pkg_do_key ( string keyword, string _file, string ebuild_opts )
{
	cout << ebuild_opts << endl;
	string cmd ( "ebuild " + ebuild_opts + " " + _file + " " + keyword + " 2> /dev/null" );
	system ( cmd.c_str ( ) );
}
#endif
