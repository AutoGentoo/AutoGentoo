/*
 * _operator.hpp
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


#include <iostream>
#include <string>
#include <vector>

using namespace std;

class _op
{
	public:
	const char* op; //!< String holding the operator string such as '?';
	bool pre_content_bool; //!< Bool asking for term before operator such as 'foo?'
	bool post_content_bool; //!< Bool asking for term before operator such as '!foo'
	string pre_content;
	string post_content;
	_op ( const char* op, bool pre_content_bool = false, bool post_content_bool = false );
	string read_pre_content ( string in )
	{
		string return_str;
		unsigned int find_op ( in.find ( op ) );
		for ( unsigned int i = find_op; i >= 0; i-- )
		{
			char x ( in.at ( i ) );
			if ( x != ' ' )
			{
				return_str += x;
			}
			else
			{
				break;
			}
		}
		return return_str;
	}
	string read_post_content ( string in )
	{
		string return_str;
		unsigned int find_op ( in.find ( op ) );
		for ( unsigned int i = find_op; i <= in.length ( ); i++ )
		{
			char x ( in.at ( i ) );
			if ( x != ' ' )
			{
				return_str += x;
			}
			else
			{
				break;
			}
		}
		return return_str;
	}
	
};

/*
REQUIRED_USE="foo? ( bar )"					If foo is set, bar must be set.
REQUIRED_USE="foo? ( !bar )"				If foo is set, bar must not be set.
REQUIRED_USE="foo? ( || ( bar baz ) )"		If foo is set, bar or baz must be set.
REQUIRED_USE="^^ ( foo bar baz )"			Exactly one of foo bar or baz must be set.
REQUIRED_USE="|| ( foo bar baz )"			At least one of foo bar or baz must be set.
REQUIRED_USE="?? ( foo bar baz )"			No more than one of foo bar or baz may be set.
*/

void init_ops ( )
{
	_op op_if ( "?", true );
	
	_op op_not ( "!", false, true );
	
	_op op_exact_one ( "^^" );
	
	_op op_least_one ( "||" );
	
	_op op_most_one ( "??" );
}
