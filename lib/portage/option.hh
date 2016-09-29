/*
 * option.hh
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos-Ubuntu>
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

#ifndef __AUTOGENTOO_PROGRAM_OPTION__
#define __AUTOGENTOO_PROGRAM_OPTION__

#include <iostream>
#include <map>
#include "_misc_tools.hh"

using namespace std;

class option
{
  public:
  string name;
  string _type;
  string value;
  bool bool_val;
  bool used;
  string desc;
  map < string, string > _map;
  
  string _long;
  string _short;
  
  string bool_toggle ( string in );
  void init ( string __long, string _default, string __short, string __type, string _desc );
  void option_sig ( string op, bool feeded );
};

class OptionSet
{
  public:
  
  string input_line;
  string program;
  string desc;
  int help_distance;
  
  map < int, option > int_to_main;
  map < string, int > str_to_long;
  map < string, int > str_to_short;
  map < int, string > str_to_val;
  map < int, bool > str_to_boolval;
  map < string, string > long_to_val;
  
  vector < string > args;
  vector < string > cmd_args;
  vector < string > help;
  
  int curr_opt;
  bool esc;
  
  void init ( string _program, string _desc, int _help_distance );
  void add_arg ( string arg );
  void add_option ( string __long, string _default, string __short, string _type, string desc );
  bool long_type ( string buff, bool feed );
  string strip ( string in );
  void feed ( string op_line );
  string space_find ( string in );
  void create_help ( );
  string operator () ( string op );
  bool operator [] ( string op );
};
#endif
