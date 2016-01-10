/*
 * warning.hpp
 * 
 * Copyright 2015 Andrei Tumbar <atadmin@Helios>
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
#include <vector>
#include <string>
#include <map>

using namespace std;

class basic {public: vector<string> full; string pkg;};
class req {public: vector< vector<string> > exactly_one; vector<string> item;};

/*! \struct use_req
 * This struct divides in the useReq warning into four parts:
 *    basic_warning (gives the basic information for the package needing an edit to use flag)
 *    use_info (information on the current global use flags)
 *    req_use (required use section, read by struct to create a make.conf edit possible)
*/

/*typedef struct use_req
{
	
	basic basic_warning;
	map<string, string> use_info;
	req req_use;
	
	void get_sections ( vector<string> input )
	{
		vector<string> basic_warning_buff;
		vector<string> use_info_buff;
		vector<string> req_use_buff;
		int currLine;
		
		for ( size_t y = currLine; input[y] != "\n"; y++ )
		{
			basic_warning_buff.push_back ( input[y] );
			currLine = y;
		}
		
		for ( size_t y = currLine; input[y] != "\n"; y++ )
		{
			use_info_buff.push_back ( input[y] );
			currLine = y;
		}
		
		for ( size_t y = currLine; input[y] != "\n"; y++ )
		{
			req_use_buff.push_back ( input[y] );
			currLine = y;
		}
		
		//Basic warning
		
		if ( basic_warning_buff.size() != 2 )
		{
			return;
		}
		basic_warning.full = basic_warning_buff;
		string basic_warning_pkg_buff = basic_warning.full[0].erase("!!! Problem resolving dependencies for");
		trim(basic_warning_pkg_buff);
		basic_warning.pkg = basic_warning_pkg_buff;
		
		//Use info
		if ( use_info_buff.size() != 2 )
		{
			return;
		}
		
		info_buff = use_info_buff[1];
		int rm_place_start(info_buff.find(basic_warning.pkg);
		int rm_place_end(info_buff.rfind(basic_warning.pkg));
		
		info_buff.erase(rm_place_start, rm_place_end);
		trim(info_buff);
		misc::remove(info_buff, "- ");
		int find_space(info_buff.find(" "));
		info_buff = info_buff.substr(find_space, info_buff.length());
		
	}
}
*/

/*
REQUIRED_USE="foo? ( bar )"					If foo is set, bar must be set.
REQUIRED_USE="foo? ( !bar )"				If foo is set, bar must not be set.
REQUIRED_USE="foo? ( || ( bar baz ) )"		If foo is set, bar or baz must be set.
REQUIRED_USE="^^ ( foo bar baz )"			Exactly one of foo bar or baz must be set.
REQUIRED_USE="|| ( foo bar baz )"			At least one of foo bar or baz must be set.
REQUIRED_USE="?? ( foo bar baz )"			No more than one of foo bar or baz may be set.

*/

template < string pkgstr >
typedef struct use_req
{
	string str = 
	
	void source ( )
	{}
	string getVal;


/*! \class Warning
 * Three types of warnings:
 *    blocks
 *    useReq (edit /etc/portage/make.conf)
 *    genericSlot (do nothing)
 */
class Warning
{
	public:
	
	string type;
	
	Warning ( vector<string> input )
	{
		for ( size_t x; x <= input.size(); x++ )
		{
			string in = input[x];
			if ( line.substr ( 0, 6 ) == string ( "[blocks" ) )
			{
				type = "blocks";
			}
			else if ( in.substr ( 0, 11 ) == "!!! Problem " )
			{
				type = "useReq";
			}
			else if ( in.substr ( 0, 11 ) == "!!! Multiple" )
			{
				type = "slot";
			}
			else
			{
				type = "generic";
			}
		}
	}
	
	void doWork ( void )
	{
		if ( type == "useReq" )
		{
			;
		}
	}
};
