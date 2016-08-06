/*
 * parse_config.hpp
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

#include "parse_config.hh"

using namespace std;

map <string, vector<string> > get_variables_split ( string input )
{
  map <string, vector<string> > buff_map;
  vector <string> variables, values, *vec;
  string buff_str;
  bool is_var = true;
  
  for ( size_t y = 0; y != input.length ( ); y++ )
  {
    if ( is_var )
    {
      vec = &variables;
    }
    else
    {
      vec = &values;
    }
    
    char curr = input[y];
    if ( is_var )
    {
      if ( curr == '=' )
      {
        is_var = false;
        vec->push_back ( buff_str );
        buff_str.clear ( );
      }
      else
      {
        buff_str += curr;
      }
    }
    if ( !is_var )
    {
      if ( curr == '\"' && ( input[y+1] == ' ' || y+1 == input.length ( ) ) )
      {
        is_var = true;
        vec->push_back ( buff_str );
        buff_str.clear ( );
      }
      else
      {
        buff_str += curr;
      }
    }
  }
  for ( size_t i = 0; i != variables.size ( ); i++ )
  {
    string var = variables[i];
    string val = values[i];
    misc::remove ( var, " " );
    misc::remove ( val, "\"" );
    misc::remove ( val, "=" );
    misc::remove ( val, "\"" );
    vector<string> _val = misc::split ( val, ' ', false );
    buff_map[var] = _val;
  }
  return buff_map;
}

string get_env ( string var )
{
  ofstream file ("temp.sh");
  string fileStr ( "#!/bin/bash\nvar=\necho $var" );
  file << fileStr;
  string cmd ( "sh temp.sh --var=$" + var + " > temp" );
  system ( cmd.c_str ( ) );
  string returnStr ( File ( "temp.sh" ).read ( ) );
  system ( "rm -rf temp" );
  system ( "rm -rf temp.sh" );
  return returnStr;
}
map < string, string > get_variables ( string input )
{
  map < string, string > buff_map;
  vector <string> variables, values, *vec;
  string buff_str;
  bool is_var = true;
  
  for ( size_t y = 0; y != input.length ( ); y++ )
  {
    if ( is_var )
    {
      vec = &variables;
    }
    else
    {
      vec = &values;
    }
    
    char curr = input[y];
    if ( is_var )
    {
      if ( curr == '=' )
      {
        is_var = false;
        vec->push_back ( buff_str );
        buff_str.clear ( );
      }
      else
      {
        buff_str += curr;
      }
    }
    if ( !is_var )
    {
      if ( curr == '\"' && ( input[y+1] == ' ' || y+1 == input.length ( ) ) )
      {
        is_var = true;
        vec->push_back ( buff_str );
        buff_str.clear ( );
      }
      else
      {
        buff_str += curr;
      }
    }
  }
  for ( size_t i = 0; i != variables.size ( ); i++ )
  {
    string var = variables[i];
    string val = values[i];
    misc::remove ( var, " " );
    misc::remove ( val, "\"" );
    misc::remove ( val, "=" );
    misc::remove ( val, "\"" );
    buff_map[var] = val;
  }
  return buff_map;
}
string get_command_str ( string cmd )
{
  string return_str;
  cmd += " > temp";
  system ( cmd.c_str ( ) );
  File f ( "temp" );
  return_str = f.read ();
  return return_str;
}
map < string, string > get_command ( string cmd )
{
  map < string, string > return_map;
  cmd += " > temp";
  system ( cmd.c_str() );
  File f ( "temp" );
  vector<string> lines = f.readlines ( );
  for ( size_t x = 0; x != lines.size ( ); x++ )
  {
    string line = lines[x];
    vector<string> divide = misc::split ( line, ':', false );
    string var, val;
    var = divide[0];
    val = divide[1];
    misc::trim ( val );
    return_map[var] = val;
  }
  system("rm -rf temp");
  return return_map;
}

vector <string> get_command_vec_raw ( string cmd )
{
  vector <string> buff;
  cmd += " > temp";
  system ( cmd.c_str ( ) );
  buff = File ( "temp" ).readlines ( );
  system("rm -rf temp");
  return buff;
}
