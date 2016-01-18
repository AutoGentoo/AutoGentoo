#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  parseFlags.py
#  
#  Copyright 2016 Andrei Tumbar <atuser@Kronos>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  


import configparser, sys

# Useful functions to help with the flag parsing
def print_dict ( _dict ):
	for x in _dict:
		sys.stdout.write(str(x))
		sys.stdout.write("=")
		print (_dict[x])
def remove_all ( string, _list ):
	for x in _list:
		string = string.replace(x, "")
	return string
def listRightIndex(alist, value):
	return len(alist) - alist[-1::-1].index(value) -1
def parse(exp):
	exp_flag = Flag(exp)
	return (exp_flag.val, exp_flag.search)
def getLetter (x, string):
	first_letter = string.rfind ("(", 0, x)
	maybe_letter = string.rfind (")", 0, x)
	if first_letter == -1:
		first_letter = 0
	if maybe_letter > first_letter:
		first_letter = maybe_letter+1
	elif first_letter != 0:
		first_letter += 1
	return first_letter
def join(between, vec):
	a = ""
	for x in vec:
		a += str(x)
		if x != vec[-1]:
			a += between
	return a
def getSet(string):
	def_dict = {}
	alt_dict = {}
	buff_set = []
	first = []
	lasts = []
	for x in range(len(string)):
		if string[x] == "(":
			first.append(x)
	for x in reversed(first):
		buff = string.find (")", x)
		if buff in lasts:
			buff = string.find(")", buff+1)
		lasts.append(buff)
		buff_set.append((x, buff))
	contents = []
	for x in buff_set:
		first_letter = getLetter(x[0], string)
		expName = string[first_letter:x[0]].strip()
		content = string[x[0]+1:x[1]].strip().split(" ")
		while "(" in content:
			a = ""
			try:
				start = content.index("(")
				end = listRightIndex(content, ")")
				end_maybe = content.index(")")
				
				a = '1'.join(content[start+1:end])
				content = [y for y in content if y not in content[start-1:end]]
			except:
				pass
			else:
				content = [y for y in content if y not in content[start-1:end]]
				content.append(alt_dict[a])
		def_dict[ first_letter ] = (content, expName)
		content_str = join("1", content)
		alt_dict[ content_str ] = first_letter
	return def_dict
def eval_flag ( flag, process ):
	if type ( flag ) == int:
		g = Flag ( process [ flag ][ 1 ] )
		if g.search:
			val_list = []
			for x in process[0]:
				val_list.append ( eval_flag ( x, process ) )
			if False in val_list:
				return ( val_list [ val_list.index ( False ) ], g )
	f = Flag ( flag )
	return ( f.end, f )
def true_num ( bool_list ):
	num = 0
	for x in bool_list:
		if x:
			num += 1
	return num
def get_true ( bool_list, alt_list ):
	buff = []
	for x in bool_list:
		if x:
			buff.append ( alt_list[bool_list.index(x)] )
	return buff
def sub_list_num ( search, _list ):
	buff = []
	a = 0
	for x in _list:
		if x == search:
			buff.append ( a )
		a += 1
	return buff
def sub_list ( search, _list, _alt_list ):
	buff = []
	a = 0
	for x in _list:
		if x == search:
			buff.append ( _alt_list [ a ] )
		a += 1
	return buff
class FlagMap:
	values = {}
	config = configparser.ConfigParser()
	required_use = ""
	def __init__ (self, filename):
		self.config.read(filename)
		for key in self.config["main"]:
			self.values[key] = self.config["main"][key] 
		self.required_use = self.config["req"]["required_use"]

class Flag:
	def __init__ (self, string):
		self.name = remove_all(string, ("^^", "!", "??", "||", "?"))
		self.ops = []
		try:
			exec("self.val = %s" % self.name)
		except:
			self.val = True
		else:
			exec("self.val = %s" % self.name)
		self.request = True
		if string[0] == "!":
			self.request = False
		self.end = self.request == self.val
		if string.replace ( self.name, "" ).replace ( "!", "" ) in ("^^", "??", "||", "?"):
			self.search = True
		else:
			self.search = False
		if string.replace ( self.name, "" ).replace ( "!", "" ) in ("^^", "??", "||", ""):
			self.must = True
		elif string.replace ( self.name, "" ) == "!":
			self.must = False
		else:
			self.must = False
		if string.replace ( self.name, "" ) in ("^^", "??", "||"):
			self.ops.append ( string.replace ( self.name, "" ) )
		if "!" in string:
			self.ops.append ( "!" )
		if "?" in string:
			self.ops.append ( "?" )
		self.string = string
class ParseFlag:
	filename = None
	required_use = None
	
	def __init__ ( self, filename ):
		self.filename = filename
		flagMap = FlagMap ( filename )
		_map = flagMap.values
		
		# Set the flag values in the 'flags' class
		for key in _map:
			keynew = key.replace ( "-", "__" )
			exec ( "global %s; %s = %s" % ( keynew, keynew, _map[ key ] ) )
		self.required_use = flagMap.required_use.replace ( "-", "__" ).replace ( "\"", "" )
		self.processed = getSet ( self.required_use )
		for x in self.processed:
			stat = Flag ( self.processed [ x ] [ 1 ] )
			p_vals = []
			p_flag = []
			p_names = []
			if stat.search:
				for y in self.processed [ x ] [ 0 ]:
					p = eval_flag ( y, self.processed )
					p_vals.append ( p[0] )
					p_flag.append ( p[1] )
					p_names.append ( y )
					if Flag(y).search and not p[0]:
						sys.stdout.write ( "(ok)" )
			if stat.ops[0] in ("^^", "??", "||"):
				if stat.ops[0] == "^^":
					num = true_num ( p_vals )
					if num != 1:
						if num > 1:
							print ( "ERROR: Exaclty one must be set %s" % p_names )
						if num < 1:
							print ( "ERROR: Set exactly one of %s" % p_names )
				if stat.ops[0] == "||":
					num = true_num ( p_vals )
					if num <= 1:
						print ( "ERROR: One must be set %s" % p_names )
				if stat.ops[0] == "??":
					num = true_num ( p_vals )
					if num > 1:
						print ( "ERROR: Unset and/or %s" % sub_list ( True, p_vals, p_names ) )
"""
REQUIRED_USE="foo? ( bar )"					If foo is set, bar must be set.
REQUIRED_USE="foo? ( !bar )"				If foo is set, bar must not be set.
REQUIRED_USE="foo? ( || ( bar baz ) )"		If foo is set, bar or baz must be set.
REQUIRED_USE="^^ ( foo bar baz )"			Exactly one of foo bar or baz must be set.
REQUIRED_USE="|| ( foo bar baz )"			At least one of foo bar or baz must be set.
REQUIRED_USE="?? ( foo bar baz )"			No more than one of foo bar or baz may be set.
"""
a = ParseFlag ( "flags" )

