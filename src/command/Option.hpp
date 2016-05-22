/*
 * Option.hpp
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


#ifndef __PROGRAM_OPTION__
#define __PROGRAM_OPTION__

#include <iostream>
#include <map>
#include "../tools/_misc_tools.hpp"

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
	
	string bool_toggle ( string in )
	{
		if ( misc::stob ( in ) )
		{
			in = "false";
		}
		else
		{
			in = "true";
		}
		return in;
	}
	
	void init ( string __long, string _default, string __short = "", string __type = "string", string _desc = "" )
	{
		_long = __long;
		_short = __short;
		_type = __type;
		value = _default;
		desc = _desc;
		used = false;
		bool_val = misc::stob ( _default );
	}
	
	void option_sig ( string op, bool feeded )
	{
		vector < string > BUFF ( misc::split ( op, '=', false ) );
		
		if ( BUFF.size ( ) == 1 and _type == "bool" )
		{
			value = bool_toggle ( value );
		}
		else if ( _type != "bool" and BUFF.size ( ) == 1 )
		{
			value = "";
		}
		else
		{
			value = BUFF [ 1 ];
		}
		
		if ( _type == "bool" )
		{
			bool_val = misc::stob ( value );
		}
		
		used = feeded;
	}
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
	
	void init ( string _program, string _desc, int _help_distance=15 )
	{
		help_distance = _help_distance;
		program = _program;
		desc = _desc;
		curr_opt = 1;
	}
	
	void add_arg ( string arg )
	{
		args.push_back ( arg );
	}
	
	void add_option ( string __long, string _default, string __short = "", string _type = "string", string desc = "Generic Option" ) 
	{
		option buff;
		buff.init ( __long, _default, __short, _type, desc );
		str_to_long [ __long ] = curr_opt;
		str_to_short [ __short ] = curr_opt;
		int_to_main [ curr_opt ] = buff;
		long_to_val [ __long ] = buff.value;
		curr_opt++;
	}
	
	bool long_type ( string buff, bool feed = true )
	{
		bool __long__;
		if ( buff.substr ( 0, 2 ) == "--" )
		{
			__long__ = true;
		}
		else if ( buff [ 0 ] == '-' )
		{
			__long__ = false;
		}
		else
		{
			if ( feed )
			{
				cmd_args.push_back ( buff );
				esc = true;
			}
			return false;
		}
		
		return __long__;
	}
	
	string strip ( string in )
	{
		if ( in [ 1 ] == '-' )
		{
			misc::removechar ( in, '-' );
			misc::removechar ( in, '-' );
		}
		else
		{
			misc::removechar ( in, '-' );
		}
		
		return in;
	}
	
	void feed ( string op_line )
	{
		input_line = op_line;
		
		vector < string > op_vec ( misc::split ( input_line, ' ', true ) );
		op_vec.erase ( op_vec.begin ( ) );
		
		for ( size_t i = 0; i != op_vec.size ( ); i++ )
		{
			string buff = op_vec [ i ];
			string __long = "";
			
			bool __long__;
			
			__long__ = long_type ( buff );
			
			if ( esc or buff.empty ( ) )
			{
				esc = false;
				continue;
			}
			
			buff = this->strip ( buff );
			
			int __INT__;
			vector < string > split_buff ( misc::split ( buff, '=', true ) );
			
			if ( __long__ )
			{
				__INT__ = str_to_long [ split_buff [ 0 ] ];
				__long = buff;
			}
			else
			{
				__INT__ = str_to_short [ split_buff [ 0 ] ];
				__long = int_to_main [ __INT__ ]._long;
			}
			option curr = int_to_main [ __INT__ ];
			
			curr.option_sig ( buff, true );
			
			int_to_main [ __INT__ ] = curr;
			str_to_val [ __INT__ ] = curr.value;
			long_to_val [ __long ] = curr.value;
			
			if ( curr._type == "bool" )
			{
				str_to_boolval [ __INT__ ] = curr.bool_val;
			}
		}
		
		if ( int_to_main [ str_to_long [ "help" ] ].used )
		{
			cout << misc::merge < string > ( help, "\n" ) << endl;
			exit (0);
		}
	}
	
	string space_find ( string in )
	{
		string out;
		int num = help_distance - in.length ( );
		
		for ( int i = 0; i != num; i++ )
		{
			out += " ";
		}
		
		return out;
	}
	
	void create_help ( )
	{
		string pkg_line ( program + ": " + desc );
		string use_line ( program );
		
		for ( size_t i = 0; i != args.size ( ); i++ )
		{
			use_line += " [" + args [ i ] + "]";
		}
		
		help.push_back ( pkg_line );
		help.push_back ( "" );
		help.push_back ( "Usage:" );
		help.push_back ( use_line );
		
		option buff;
		this->add_option ( "help", "", "h", "none", "Display help" );
		
		help.push_back ( "" );
		help.push_back ( "Options:" );
		
		for ( map < int, option >::iterator i = int_to_main.begin ( ); i != int_to_main.end ( ); i++ )
		{
			string buff ( "  --" + i->second._long + space_find ( i->second._long ) + "-" + i->second._short + "			" + i->second.desc );
			help.push_back ( buff );
		}
		
		string help_str ( misc::merge < string > ( help, "\n" ) );
		
		buff.option_sig ( string ( "help=" + help_str ), false );
	}
	
	string operator () ( string op )
	{
		bool is_long;
		int buff;
		
		if ( op.length ( ) > 1 )
		{
			is_long = true;
		}
		else
		{
			is_long = false;
		}
		
		if ( is_long )
		{
			buff = str_to_long [ op ];
		}
		
		if ( !is_long )
		{
			buff = str_to_short [ op ];
		}
		
		return int_to_main [ buff ].value;
	}
	
	bool operator [] ( string op )
	{
		bool is_long;
		int buff;
		
		if ( op.length ( ) > 1 )
		{
			is_long = true;
		}
		else
		{
			is_long = false;
		}
		
		if ( is_long )
		{
			buff = str_to_long [ op ];
		}
		
		if ( !is_long )
		{
			buff = str_to_short [ op ];
		}
		
		return int_to_main [ buff ].bool_val;
	}
};
#endif
