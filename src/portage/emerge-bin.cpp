/*
 * emerge.cxx
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
#include "emerge.hh"
#include "option.hh"
#include "_misc_tools.hh"

using namespace std;

int main(int argc, char* args[])
{
  string input;
  
  for ( int i = 0; i != argc; i++ )
  {
    input += args [ i ];
    input += " ";
  }
  
  
  OptionSet emerge_opts;
  emerge_opts.init ( "emerge", "Use the AutoGentoo portage API to install specified packages", 15 );
  
  emerge_opts.add_arg ( "PACKAGE" );
  emerge_opts.add_arg ( "OPTIONS" );
  
  string name;
  if ( string ( args [ 1 ] ).find ( "/" ) != string::npos )
  {
    name = string ( args [ 1 ] ).substr ( string ( args [ 1 ] ).find ( "/" ) + 1, string ( args [ 1 ] ).length ( ) - string ( args [ 1 ] ).find ( "/" ) - 1 );
  }
  else
  {
    name = args [ 1 ];
  }
  
  string pretend = name;
  pretend += ".emerge";
  string config = name;
  config += ".cfg";
  emerge_opts.add_option ( "emerge", pretend, "e", "string", "Specify where the output of emerge --pretend is kept" );
  emerge_opts.add_option ( "config", config, "c", "string", "Specify where the output of package config is kept" );
  emerge_opts.add_option ( "pretend", "true", "p", "bool", "Specify whether to execute emerge --pretend" );
  emerge_opts.add_option ( "install", "true", "i", "bool", "Specify whether to actually install or whether to just configure" );
  emerge_opts.add_option ( "sync", "false", "g", "bool", "Sync the portage tree and exit (run emerge --sync)" );
  emerge_opts.add_option ( "order", "None", "r", "string", "Change the stage order of installation" );
  emerge_opts.add_option ( "options", "", "o", "string", "Select options run with emerge" );
  emerge_opts.add_option ( "updates", "false", "u", "bool", "Install updates only" );
  emerge_opts.add_option ( "ebuild-opts", "--color=y", "O", "string", "Select options run with ebuild" );
  emerge_opts.add_option ( "show-opts", "false", "s", "bool", "Show the current environment variables and exit" );
  
  emerge_opts.create_help ( );
  emerge_opts.feed ( input );
  
  if ( emerge_opts [ "show-opts" ] )
  {
    for ( map < int, option >::iterator i = emerge_opts.int_to_main.begin ( ); i != emerge_opts.int_to_main.end ( ); i++ )
    {
      cout << i->second._long << ": " << i->second.value << endl;
    }
    misc::print_vec ( emerge_opts.cmd_args );
    exit (0);
  }
  
  if ( emerge_opts [ "sync" ] )
  {
    system ( "emerge --sync" );
    exit ( 0 );
  }
  
  Emerge ( emerge_opts.cmd_args [ 0 ], emerge_opts ( "emerge" ), emerge_opts ( "config" ), emerge_opts [ "pretend" ], emerge_opts ( "options" ), emerge_opts [ "updates" ] );
  
  if ( emerge_opts [ "install" ] )
  {
    string cmd ( "python3 package.py " + emerge_opts ( "config" ) + " logs " + emerge_opts ( "order" ) + " false false " + emerge_opts ( "ebuild-opts" ) );
    system ( cmd.c_str ( ) );
  }
  return 0;
}
