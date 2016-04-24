/*
 * parser.hpp
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

#ifndef __AUTOGENTOO_PARSER__
#define __AUTOGENTOO_PARSER__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "signal.hpp"

class Parser
{
	public:
	vector <string> keywords;
	vector <AG_SIGNAL> signals;
	vector <string> __FILE__;
	string path;
	map <string, AG_SIGNAL> k_to_s;
	vector <AG_SIGNAL> sent_signals;
	
	void init_map ( )
	{
		for ( size_t i = 0; i != keywords.size ( )
	
	void feed ( string _path, 
#endif
