/*
 * stringEdit.hpp
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
#include <string>
#include <vector>
#include <algorithm>

namespace strfmt
{
	void remove ( std::string &removing, const char *toRemove )
	{
		std::string::size_type foundAt = removing.find ( toRemove );
		if ( foundAt != std::string::npos )
		{
			removing.erase ( foundAt, sizeof ( toRemove ) );
		}
	}
	
	void removeNewLine ( std::string &str )
	{
		str.erase(str.length());
	}
	void replace ( std::string &replacing, const char *oldstr, const char * newstr )
	{
		std::string::size_type foundOld = replacing.find ( oldstr );
		if ( foundOld != std::string::npos )
		{
			replacing.replace ( foundOld, sizeof ( oldstr ), newstr );
		}
	}
	
	std::string getSubStr ( std::string &input, int startIndex, const char *exitChar )
	{
		unsigned int currentIndex = startIndex;
		while ( currentIndex <= input.length ( ) && input.at ( currentIndex ) != std::string ( exitChar ).at ( 0 ) )
		{
			++currentIndex;
		}
		++currentIndex;
		return input.substr ( startIndex, currentIndex );
	}
	
	int rfind ( std::string &input, const char *find )
	{
		for ( unsigned int i=input.length( ); i < input.length( ); --i )
		{
			if ( input.at ( i ) == std::string ( find  ).at ( 0 ) )
			{
				return i;
			}
		}
		
		return -1;
	}
	
	template<class vectorType>
	int find ( std::vector<vectorType> &input, std::string findstr )
	{
		int x = 0;
		for ( std::string y = input[x]; sizeof ( input ); x++ )
		{
			if ( y == findstr )
			{
				return x;
			}
		}
		return -1;
	}
}
