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

def find_between( s, first, last ):
	try:
		start = s.index( first ) + len( first )
		end = s.index( last, start )
		return s[start:end]
	except ValueError:
		return ""
def find_between_r( s, first, last ):
	try:
		start = s.rindex( first ) + len( first )
		end = s.rindex( last, start )
		return s[start:end]
	except ValueError:
		return ""
def find_between_sr( s, first="(", last=")" ):
	start = s.index( first )
	end = s.rindex( last )
	return s[start:end]
def remove_all ( string, _list ):
	for x in _list:
		string = string.replace(x)
	return string

class FlagMap:
	values = {}
	config = configparser.ConfigParser()
	def __init__ (self, filename):
		config.read(filename)
		for key in config["main"]:
			values[key] = config["main"][key]
class Flag:
	def __init__ (self, string):
		self.name = remove_all(string, ("^^", "!", "??", "||", "?"))
		try:
			exec("val = flags.%s" % self.name)
		except:
			val = True
		
		if string[0] == "!":
			self.val = not val
		if string in ("^^", "??", "||", "?"):
			self.search = True
def listRightIndex(alist, value):
	return len(alist) - alist[-1::-1].index(value) -1
def parse(exp):
	exp_flag = Flag(exp)
	return (exp_flag.val, exp_flag.search)
def getSet(string):
	"""
	a ( || ( adc ) )
	
	
	a ( acb )
	  2     8
	  2:8 (buff_dict)
	  [2 , 8]
	  a : [2,8]
	"""
	set_dict = {}
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
		print (string[x[0]:x[1]])
		first_letter = string.find ("(", x[0])
		maybe_letter = string.rfind (")", 0, x[0])
		if first_letter == -1:
			first_letter = 0
		if maybe_letter > first_letter:
			first_letter = maybe_letter
		elif first_letter != 0:
			first_letter += 1
		
		
		
		expName = string[first_letter:x[0]].strip()
		content = string[x[0]:x[1]].strip().split(" ")
		
		if "(" in content:
			try:
				start = content.index("(")
				end = listRightIndex(content, ")")
				content = [y for y in content if y not in content[start:end]]
			except:
				pass
			else:
				content = [y for y in content if y not in content[start:end]]
		set_dict[ first_letter ] = (content, expName)
	return set_dict


class flags:
	pass

class ParseFlag:
	
	filename = None
	required_use = None
	div = [0]
	strings = []
	_map = ""
	
	def __init__ (self, filename, required_use):
		_map = FlagMap ( filename ).values
		#modemmanager? ( ppp ) !wext? ( wifi ) ^^ ( nss gnutls ) ^^ ( dhclient dhcpcd )
		p = 0
		already_p = False;
		for x in required_use:
			if x == "(":
				already_p = True
				p += 1
			if x == ")":
				p += -1
			if p == 0 and already_p:
				div.append(x)
		for key in _map:
			exec ("flags.%s = %s" % (key, _map[key]))
		for x in div[1:]:
			string.append(required_use[div.find(x)-1:div[x]])
		#for x in string:
def print_dict ( _dict ):
	for x in _dict:
		sys.stdout.write(str(x))
		sys.stdout.write("=")
		print (_dict[x])
print_dict(getSet("modemmanager? ( ppp ) !wext? ( wifi ) ^^ ( nss gnutls ) ^^ ( dhclient dhcpcd ) foo? ( || ( bar baz ) )"))
