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
#include <boost/property_tree/ptree.hpp>
#include <boost/format.hpp>

using boost::format;
using boost::io::group;

/**\namespace misc
 * The strfmt namespace consists of a various collection of tools
 * to format string
 *
 */
#ifndef MISC
#define MISC
namespace misc
{
	void remove ( std::string &removing, std::string toRemove )
	{
		std::string::size_type foundAt = removing.find ( toRemove );
		if ( foundAt != std::string::npos )
		{
			removing.erase ( foundAt, toRemove.length ( ) );
		}
	}
	void removechar ( std::string &removing, char toRemove )
	{
		std::string::size_type foundAt = removing.find ( toRemove );
		if ( foundAt != std::string::npos )
		{
			removing.erase ( foundAt, 1 );
		}
	}
	void rremovechar ( std::string &removing, char toRemove )
	{
		std::string::size_type foundAt = removing.rfind ( toRemove );
		if ( foundAt != std::string::npos )
		{
			removing.erase ( foundAt, 1 );
		}
	}
	void removeNewLine ( std::string &str )
	{
		str.erase(str.length());
	}
	void replace ( std::string &replacing, std::string oldstr, std::string newstr )
	{
		std::string::size_type foundOld = replacing.find ( oldstr );
		if ( foundOld != std::string::npos )
		{
			replacing.replace ( foundOld, sizeof ( oldstr ), newstr );
		}
	}
	std::string getSubStr ( std::string &input, int startIndex, char exitChar )
	{
		size_t currentIndex = startIndex;
		while ( currentIndex != input.length ( ) && input.at ( currentIndex ) != exitChar )
		{
			++currentIndex;
		}
		return input.substr ( startIndex, currentIndex-startIndex+1 );
	}
	
	int getSubStrInt ( std::string input, int startIndex, const char *exitChar )
	{
		size_t currentIndex = startIndex;
		while ( currentIndex != input.length ( ) && input.at ( currentIndex ) != std::string ( exitChar ).at ( 0 ) )
		{
			++currentIndex;
		}
		++currentIndex;
		return currentIndex;
	}
	std::string substr ( std::string input, int start, int end )
	{
		return input.substr(start, end-start);
	}
	int rfind ( std::string input, char find )
	{
		for ( size_t i=input.length( ) - 1; i != 0; --i )
		{
			if ( input[ i ] == find )
			{
				return i;
			}
		}
		
		return -1;
	}
	
	template < class vectorType >
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
	
	int strfind ( std::string in, char findchar, int startchar = 0 )
	{
		size_t x = startchar;
		std::cout << "x=" << x << std::endl;
		std::cout << "len=" << in.length ( ) << std::endl;
		if ( x >= in.length ( ) )
		{
			return -1;
		}
		for ( char y = in.at ( x ); x != in.length ( ); x++ )
		{
			y = in.at ( x );
			if ( y == findchar )
			{
				return x;
			}
		}
		return -1;
	}
	
	template < class type >
	type formatini ( std::string in, boost::property_tree::ptree pt )
	{
		std::vector<int> findvec;
		int find = 0;
		while ( find != -1 )
		{
			find = in.find( "$", find+1 );
			if ( find != -1 )
			{
				findvec.push_back ( find );
			}
			else
			{
				break;
			}
		}
		std::vector<std::string> valvec;
		for ( size_t x = 0; x != findvec.size ( ) - 1; x++ )
		{
			if ( x == findvec.size ( ) )
			{
				break;
			}
			int y = findvec [ x ];
			std::string buff = misc::getSubStr ( in, y, '}' );
			misc::remove( buff, "$" );
			misc::remove( buff, "{" );
			misc::remove( buff, "}" );
			valvec.push_back ( buff );
		}
		misc::removechar ( in, '"' );
		misc::removechar ( in, '"' );
		for ( size_t x = 0; x != valvec.size ( ); x++ )
		{
			std::string y = valvec [ x ];
			type buff = pt.get<type> ( y );
			misc::removechar ( buff, '"' );
			misc::removechar ( buff, '"' );
			std::string valrep = str ( format( "${%1%}" ) % y );
			in.replace ( in.find( valrep ), valrep.length ( ), buff );
		}
		return in;
	}
	
	std::vector<std::string> split ( std::string str, char chr )
	{
		std::vector<std::string> returnList;
		std::string buff;
		for ( size_t y = 0; y != str.length ( ); y++ )
		{
			char curr = str[y];
			if ( curr == chr )
			{
				returnList.push_back ( buff );
				buff.clear ( );
				continue;
			}
			buff += curr;
		}
		
		return returnList;
	}
	
	std::string merge ( std::vector<std::string> in )
	{
		std::string return_buff;
		for ( size_t i; i != in.size ( ); i++ )
		{
			return_buff.append ( in[i] );
		}
		return return_buff;
	}
	
	int stoi ( std::string in )
	{
		std::istringstream convert ( in );
		int val;
		convert >> val;
		return val;
	}
}
#endif
