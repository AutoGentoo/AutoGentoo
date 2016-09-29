/*
 * ini_parser.cxx
 * 
 * Copyright 2015 Andrei Tumbar <atuser@Kronos>
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
#include <typeinfo>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "../tools/file.hpp"
#include "../tools/_misc_tools.hpp"

class iniparser
{
  public:
  boost::property_tree::ptree pt;
  std::vector<std::string> file, vars;
  std::string filename;
  
  void read ( std::string filename )
  {
    file = File ( filename ).readlines ( );
    boost::property_tree::ini_parser::read_ini(filename, pt);
    
    /*boost::property_tree::ptree::const_iterator end = pt.end();
    for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != end; ++it)
    {
      //std::string result_string = it[1].str();
      std::cout << it << std::endl;
    }
    BOOST_FOREACH(bpt::value_type& v, pt) 
    {
      pid = v.second.get<int>("pid"); // use the converting path getter
      .push_back(pid);
    }*/
  }
  
  template < class intype >
  intype get ( std::string val )
  {
    intype buff = pt.get<intype>( val );
    buff = misc::formatini<intype> ( buff, pt );
    return buff;
  }
};
