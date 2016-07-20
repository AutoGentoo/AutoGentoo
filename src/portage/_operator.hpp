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
#include <exception>
#include <boost/algorithm/string.hpp>
#include "_misc_tools.hpp"

using namespace boost::algorithm;
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
  int requiredVal;
  
  _op ( string op, bool hasPreContent = false, bool hasPostContent = false, int requiredVal = 2, string content_str = "" )
  {
    if ( !content_str.empty ( ) )
    {
      if ( hasPreContent )
      {
        pre_content = read_pre_content ( content_str );
      }
      
      if ( hasPostContent )
      {
        post_content = read_post_content ( content_str );
      }
      
      get_content ( content_str );
    }
  }
  
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
  
  void get_content ( string in )
  {
    int getSpace = in.find ( " " );
    string content_str ( misc::substr ( in, getSpace + 1, in.length ( ) ) );
    trim ( content_str );
    content = misc::split ( content_str, ' ' );
  }
};

/*
REQUIRED_USE="foo? ( bar )"          If foo is set, bar must be set.
REQUIRED_USE="foo? ( !bar )"        If foo is set, bar must not be set.
REQUIRED_USE="foo? ( || ( bar baz ) )"    If foo is set, bar or baz must be set.
REQUIRED_USE="^^ ( foo bar baz )"      Exactly one of foo bar or baz must be set.
REQUIRED_USE="|| ( foo bar baz )"      At least one of foo bar or baz must be set.
REQUIRED_USE="?? ( foo bar baz )"      No more than one of foo bar or baz may be set.
*/

class op_if: public _op
{
  public:
  op_if ( string str ) :  _op ( "?", true, false, 1, str ) {}
};
class op_not: public _op
{
  public:
  op_not ( string str ) :  _op ( "!", false, true, 0, str ) {}
};
class op_exact_one: public _op
{
  public:
  op_exact_one ( string str ) :  _op ( "^^", false, false, 2, str ) {}
};
class op_least_one: public _op
{
  public:
  op_least_one ( string str ) :  _op ( "||", false, false, 2, str ) {}
};
class op_most_one: public _op
{
  public:
  op_most_one ( string str ) :  _op ( "??", false, false, 2, str ) {}
};

class getOps
{
  public:
  bool search, neg, exact, least, noMore;
  string useflag;
  
  getOps ( string UseFlag )
  {
    if ( UseFlag [ 0 ] == '!' )
    {
      neg = true;
    }
    if ( UseFlag [ UseFlag.length ( ) == '?' )
    {
      search = true;
    }
    if ( UseFlag == "^^" )
    {
      exact = true;
    }
    if ( UseFlag == "||" )
    {
      least = true;
    }
    if ( UseFlag == "??" )
    {
      noMore = true;
    }
    if ( neg && search )
    {
      useflag = misc::substr ( UseFlag, 1, UseFlag.length ( ) - 1 );
    }
  }
};

void neg ( bool &a )
{
  a = !a;
}

