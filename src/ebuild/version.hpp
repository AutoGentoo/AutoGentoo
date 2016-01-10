/*
 * use.hpp
 * 
 * Copyright 2016 Andrei Tumbar <atadmin@Helios>
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
#include <string>
#include <vector>
#include "../emerge/formatString.hpp"

using namespace std;

class version
{
	public:
	vector<int> v; //!< Vector of the version, string divided by '.' for example 1.2.3 = [1, 2, 3]
	string revision;
	int revision_num;
	bool has_revision;
	string in_str;
	
	version ( string in_str )
	{
		vector<string> buff;
		if ( in_str[in_str.length ( ) - 2] == 'r' )
		{
			has_revision = true;
			revision = in_str.substr( in_str.length ( ) - 2, in_str.length ( ) );
			revision_num = revision[in_str.length ( )];
		}
		
		string buff_in = in_str;
		
		if ( has_revision )
		{
			strfmt::remove ( buff_in, revision );
		}
		
		vector<string> buff_vec ( strfmt::split ( buff_in, '.' ) );
		
		for ( size_t i; i <= buff_vec.size ( ); i++ )
		{
			v.push_back ( strfmt::stoi ( buff_vec[i] ) );
		}
	}
	bool operator < ( version compare )
	{
		version longer = *this;
		if ( v.size ( ) < compare.v.size ( ) )
		{
			longer = compare;
		}
		if ( v.size ( ) > compare.v.size ( ) )
		{
			longer = *this;
		}
		
		//Compare the main version number
		for ( size_t i; i <= longer.v.size ( ); i++ )
		{
			int curr_num_one = v[i];
			int curr_num_two = compare.v[i];
			if ( curr_num_one < curr_num_two )
			{
				return true;
			}
			if ( curr_num_one > curr_num_two )
			{
				return false;
			}
		}
		
		bool one_has_revision = has_revision;
		bool two_has_revision = compare.has_revision;
		if ( one_has_revision && two_has_revision )
		{
			if ( revision_num >= compare.revision_num )
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		if ( one_has_revision && !two_has_revision )
		{
			return false;
		}
		if ( !one_has_revision && two_has_revision )
		{
			return true;
		}
		if ( !one_has_revision && !two_has_revision )
		{
			return true;
		}
	}
	bool operator <= ( version compare )
	{
		version longer = *this;
		if ( v.size ( ) < compare.v.size ( ) )
		{
			longer = compare;
		}
		if ( v.size ( ) > compare.v.size ( ) )
		{
			longer = *this;
		}
		
		//Compare the main version number
		for ( size_t i; i <= longer.v.size ( ); i++ )
		{
			int curr_num_one = v[i];
			int curr_num_two = compare.v[i];
			if ( curr_num_one < curr_num_two )
			{
				return true;
			}
			if ( curr_num_one > curr_num_two )
			{
				return false;
			}
		}
		
		bool one_has_revision = has_revision;
		bool two_has_revision = compare.has_revision;
		if ( one_has_revision && two_has_revision )
		{
			if ( revision_num > compare.revision_num )
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		if ( one_has_revision && !two_has_revision )
		{
			return false;
		}
		if ( !one_has_revision && two_has_revision )
		{
			return true;
		}
		if ( !one_has_revision && !two_has_revision )
		{
			return true;
		}
	}
	bool operator >= ( version compare )
	{
		version longer = *this;
		if ( v.size ( ) < compare.v.size ( ) )
		{
			longer = compare;
		}
		if ( v.size ( ) > compare.v.size ( ) )
		{
			longer = *this;
		}
		
		//Compare the main version number
		for ( size_t i; i <= longer.v.size ( ); i++ )
		{
			int curr_num_one = v[i];
			int curr_num_two = compare.v[i];
			if ( curr_num_one < curr_num_two )
			{
				return false;
			}
			if ( curr_num_one > curr_num_two )
			{
				return true;
			}
		}
		
		bool one_has_revision = has_revision;
		bool two_has_revision = compare.has_revision;
		if ( one_has_revision && two_has_revision )
		{
			if ( revision_num > compare.revision_num )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		if ( one_has_revision && !two_has_revision )
		{
			return true;
		}
		if ( !one_has_revision && two_has_revision )
		{
			return false;
		}
		if ( !one_has_revision && !two_has_revision )
		{
			return false;
		}
	}
	bool operator > ( version compare )
	{
		version longer = *this;
		if ( v.size ( ) < compare.v.size ( ) )
		{
			longer = compare;
		}
		if ( v.size ( ) > compare.v.size ( ) )
		{
			longer = *this;
		}
		
		//Compare the main version number
		for ( size_t i; i <= longer.v.size ( ); i++ )
		{
			int curr_num_one = v[i];
			int curr_num_two = compare.v[i];
			if ( curr_num_one < curr_num_two )
			{
				return false;
			}
			if ( curr_num_one > curr_num_two )
			{
				return true;
			}
		}
		
		bool one_has_revision = has_revision;
		bool two_has_revision = compare.has_revision;
		if ( one_has_revision && two_has_revision )
		{
			if ( revision_num >= compare.revision_num )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		if ( one_has_revision && !two_has_revision )
		{
			return true;
		}
		if ( !one_has_revision && two_has_revision )
		{
			return false;
		}
		if ( !one_has_revision && !two_has_revision )
		{
			return false;
		}
	}
	bool operator = ( version compare )
	{
		if ( v == compare.v && revision_num == compare.revision_num )
		{
			return true;
		}
		return false;
	}
};
