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
	bool hasPreContent; //!< Bool asking for term before operator such as 'foo?'
	bool hasPostContent; //!< Bool asking for term before operator such as '!foo'
	string pre_content;
	string post_content;
	string content_str;
	vector<string> content;
	_op ( const char* op, bool hasPreContent = false, bool hasPostContent = false );
	string read_pre_content ( string in )
	{
		string return_str;
		size_t find_op ( in.find ( op ) );
		for ( size_t i = find_op; i >= 0; i-- )
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
		size_t find_op ( in.find ( op ) );
		for ( size_t i = find_op; i <= in.length ( ); i++ )
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
	
	//void get_content ( string in )
	//{
	//	if 
};

/*
REQUIRED_USE="foo? ( bar )"					If foo is set, bar must be set.
REQUIRED_USE="foo? ( !bar )"				If foo is set, bar must not be set.
REQUIRED_USE="foo? ( || ( bar baz ) )"		If foo is set, bar or baz must be set.
REQUIRED_USE="^^ ( foo bar baz )"			Exactly one of foo bar or baz must be set.
REQUIRED_USE="|| ( foo bar baz )"			At least one of foo bar or baz must be set.
REQUIRED_USE="?? ( foo bar baz )"			No more than one of foo bar or baz may be set.
*/

class op_if: public _op
{
	public:
	op_if ( string content_str ) :  _op ( "?", true ) {}
};
class op_not: public _op
{
	public:
	op_not ( string content_str ) :  _op ( "!", false, true ) {}
};
class op_exact_one: public _op
{
	public:
	op_exact_one ( string content_str ) :  _op ( "^^" ) {}
};
class op_least_one: public _op
{
	public:
	op_least_one ( string content_str ) :  _op ( "||" ) {}
};
class op_most_one: public _op
{
	public:
	op_most_one ( string content_str ) :  _op ( "??" ) {}
};
