/*
 * _misc_tools.hpp
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

#include "_misc_tools.hh"

void misc::remove ( std::string &removing, std::string toRemove )
{
  std::string::size_type foundAt = removing.find ( toRemove );
  if ( foundAt != std::string::npos )
  {
    removing.erase ( foundAt, toRemove.length ( ) );
  }
}
std::string misc::remove_r ( std::string removing, std::string toRemove )
{
  std::string::size_type foundAt = removing.find ( toRemove );
  if ( foundAt != std::string::npos )
  {
    removing.erase ( foundAt, toRemove.length ( ) );
  }
  return removing;
}
void misc::removechar ( std::string &removing, char toRemove )
{
  std::string::size_type foundAt = removing.find ( toRemove );
  if ( foundAt != std::string::npos )
  {
    removing.erase ( foundAt, 1 );
  }
}
void misc::rremovechar ( std::string &removing, char toRemove )
{
  std::string::size_type foundAt = removing.rfind ( toRemove );
  if ( foundAt != std::string::npos )
  {
    removing.erase ( foundAt, 1 );
  }
}
void misc::removeNewLine ( std::string &str )
{
  str.erase(str.length());
}
void misc::replace ( std::string &replacing, std::string oldstr, std::string newstr )
{
  std::string::size_type foundOld = replacing.find ( oldstr );
  if ( foundOld != std::string::npos )
  {
    replacing.replace ( foundOld, sizeof ( oldstr ), newstr );
  }
}
std::string misc::getSubStr ( std::string &input, int startIndex, char exitChar )
{
  size_t currentIndex = startIndex;
  while ( currentIndex != input.length ( ) && input.at ( currentIndex ) != exitChar )
  {
    ++currentIndex;
  }
  return input.substr ( startIndex, currentIndex-startIndex+1 );
}

int misc::getSubStrInt ( std::string input, int startIndex, const char *exitChar )
{
  size_t currentIndex = startIndex;
  while ( currentIndex != input.length ( ) && input.at ( currentIndex ) != std::string ( exitChar ).at ( 0 ) )
  {
    ++currentIndex;
  }
  ++currentIndex;
  return currentIndex;
}
std::string misc::substr ( std::string input, int start, int end )
{
  if (end == -1)
  {
    return input.substr(start, input.length () - start);
  }
  return input.substr(start, end-start);
}
int misc::rfind ( std::string input, char find )
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

template < class T >
int misc::find ( std::vector < T > input, T findstr )
{
  for ( size_t x = 0; x != input.size ( ); x++ )
  {
    T y = input [ x ];
    if ( y == findstr )
    {
      return x;
    }
  }
  return -1;
}

int misc::find_spec ( std::vector < std::string > input, std::string findstr, int start, int end )
{
  for ( size_t x = 0; x != input.size ( ); x++ )
  {
    std::string y = misc::substr ( input [ x ], start, end );
    if ( y == findstr )
    {
      return x;
    }
  }
  return -1;
}

int misc::strfind ( std::string in, char findchar, int startchar = 0 )
{
  size_t x = startchar;
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
type misc::formatini ( std::string in, boost::property_tree::ptree pt )
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

std::vector<std::string> misc::split ( std::string str, char chr, bool _trim = false )
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
    if ( _trim )
    {
      if ( curr != ' ' )
      {
        buff += curr;
      }
    }
    else
    {
      buff += curr;
    }
  }
  returnList.push_back ( buff );
  return returnList;
}

std::string misc::itos ( int i )
{
  std::string s;
  std::stringstream convert;
  convert << i;
  s = convert.str();
  return s;
}

std::string misc::merge ( std::vector<std::string> in, std::string between )
{
  std::string return_buff;
  for ( size_t i = 0; i != in.size ( ); i++ )
  {
    return_buff += in[i];
    if ( i + 1 != in.size ( ) )
    {
      return_buff.append ( between );
    }
  }
  return return_buff;
}

int misc::stoi ( std::string in )
{
  std::istringstream convert ( in );
  int val;
  convert >> val;
  return val;
}

bool misc::stob ( std::string in )
{
  boost::algorithm::to_lower ( in );
  if ( in == "false" )
  {
    return false;
  }
  
  if ( in == "true" )
  {
    return true;
  }
  
  return false;
}

std::string misc::getOutput ( std::string command )
{
  std::string cmd ( command + " > temp" );
  std::system ( cmd.c_str ( ) );
  std::ofstream _file;
  _file.open ( "temp" );
  std::stringstream buff;
  buff << _file.rdbuf();
  std::system ( "rm -rf temp" );
  return buff.str ( );
}

std::vector < std::string > misc::splitByVec ( std::string input, std::vector < int > vec )
{
  std::vector < std::string > returnVec;
  
  int currBuff = 0;
  
  for ( size_t currNum = 0; currNum != vec.size ( ); currNum++ )
  {
    int curr = vec [ currNum ];
    returnVec.push_back ( misc::substr ( input, currBuff, curr ) );
    currBuff = curr;
  }
  
  return returnVec;
}

void misc::print_vec ( std::vector < std::string > in )
{
  for ( size_t i = 0; i != in.size ( ); i++ )
  {
    std::cout << in [ i ] << std::endl;
  }
}

template < class T >
std::vector < T > misc::set_vec ( std::vector < T > in, size_t pos, T val )
{
  std::vector < T > buffVec;
  for ( size_t i = 0; i != in.size ( ); i++ )
  {
    if ( i == pos )
    {
      buffVec.push_back ( val );
    }
    else
    {
      buffVec.push_back ( in [ i ] );
    }
  }
  return buffVec;
}

template < class V, class T >
bool misc::in ( V VEC, T VAL )
{
  for ( size_t i; i != VEC.size ( ); i++ )
  {
    T curr = VEC [ i ];
    if ( curr == VAL )
    {
      return true;
    }
  }
  return false;
}
