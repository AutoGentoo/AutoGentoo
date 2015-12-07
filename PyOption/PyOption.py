#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  PyOption.py
#  
#  Copyright 2015 Andrei Tumbar <atuser@Kronos>
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

from Exceptions import *
import os, sys

def HelpArgument(string, color, open_char="[ ", close_char=" ]"):
	esc_seq="\x1b["
	end=("%s39;49;00m" % (esc_seq))
	red=("%s31;01m" % (esc_seq))
	green=("%s32;01m" % (esc_seq))
	yellow=("%s33;01m" % (esc_seq))
	blue=("%s34;01m" % (esc_seq))
	magenta=("%s35;01m" % (esc_seq))
	cyan=("%s36;01m" % (esc_seq))
	bold = ("\033[1m")
	exec("color = %s" % color)
	char = [open_char, close_char]
	return "%s%s%s%s%s%s" % (char[0], bold, color, string, end, char[1])


class Option:
	def __init__(self, _short, _full=None, _desc=None, default=False):
		_short = _short.replace("-", "")
		if len(_short) != 1:
			raise OptionError("Option type 'short' cannot be longer than one char")
		self.short = "-%s" % _short
		if _full:
			_full = _full.replace("-", "")
			if len(_full) <= 1:
				raise OptionError("Argument 'full' needs to be longer than one char")
			self.full = full(_full)
		else:
			self.full = _full
		self.desc = _desc
		self.called = default
	
	def get_used(self, arg=sys.argv):
		if self.short in arg or self.full in arg:
			self.called = True
		return self.called

class OptionWithArgs(Option):
	def __init__(self, _short, _full=None, _desc=None, numofargs=1, argtype=str, default=False):
		# arg is the list for the options arguments
		# arguments will be appended to this as long as the bash arg is not
		# a valid option
		#
		#                       1st arg 2nd arg, etc.
		# arg structure will be: [arg1,   arg2]
		# values in this list will be set to the command line argument number
		self.arg = []
		
		# arg_names is a set that corresponds to arg and indicates the name of each argument
		# This will used when you use the 'createHelp' object in OptionSet
		self.arg_names = []
		
		# max_arg is set to 0 by default
		# This means that the option does not accept arguments
		# If argument is given then an ArgumentError will sent
		self.max_arg = 0
		_short = _short.replace("-", "")
		if len(_short) != 1:
			raise OptionError("Option type 'short' cannot be longer than one char")
		self.short = "-%s" % _short
		if _full:
			_full = _full.replace("-", "")
			if len(_full) <= 1:
				raise OptionError("Argument 'full' needs to be longer than one char")
			self.full = full(_full)
		else:
			self.full = _full
		self.desc = _desc
		self.called = default
	def new_arg(self, default_value="", option_name="arg"):
		# Append default_value to the self.arg array
		self.arg.append(default_value)
		
		# Add 1 to self.max_arg to allow another argument
		self.max_arg += 1
		
		# Set append the option_name to self.arg_name array
		self.arg_names.append(option_name)
		
	def set_args(self, arg_array, name_array):
		# Set the self.args to the given array
		self.arg = array
		self.arg_names = name_array
		
		# Set the max args to the length of array
		self.max_arg = len(array)
	def ParseArgs(self, arguments=sys.argv):
		if self.get_used():
			for x in argument:
				if x in [self.short, self.full]:
					startArg = argument.index(x)
					for y in range(startArg, startArg+self.max_arg):
						self.arg[y] = argument[y]

class Section:
	def __init__(self, arguments=[], title=None):
		self.args = ' '.join(arguments)
		if title:
			self.string = '\033[1m\x1b[36;01m%s\x1b[39;49;00m %s' % (title, self.args)
		else:
			self.string = '   %s' % self.args
class SectionFromOption:
	def __init__(self, option, prechar="   "):
		if not isinstance(option, Option.Option) or not isinstance(option, Option.OptionWithArgs):
			raise SectionError("Value '%s' is not an Option or OptionWithArgs instance" % option)
		if option.full:
			self.option = option.full
			self.alt = "(%s)" % self.option
		else:
			self.option = option.short
			self.alt = None
		if isinstance(option, Option.OptionWithArgs):
			if len(option.arg) > 1:
				self.args = []
				for x in option.arg_name:
					self.args.append('\033[1m\x1b[36;01m%s\x1b[39;49;00m' % x)
				self.arg_string = "< %s >" % ' | '.join(self.args)
			else:
				self.arg_string = '\033[1m\x1b[36;01m%s\x1b[39;49;00m' % option.arg_name[0]
		else:
			self.arg_string = None
		self.string = "%s%s %s %s		%s" % (prechar, self.option, self.alt, self.arg_string, option.desc)
		self.string = self.string.replace("None", "")

class HelpCat:
	def __init__(self, string, title=None):
		self.string = string
		self.title = title
	def divide_string(self):
		self.string_array = self.string.split("\n")
	def get_title(self):
		return sections[0][0:sections[0].index(":")-1]
class HelpCatFromSections(HelpCat):
	def __init__(self, title, sections): 
		for x in sections:
			if not isinstance(x, str):
				raise HelpCatError("Argument '%s' is not an str or str generated from Section/SectionFromOption" % x)
		if not title:
			self.title = sections[0][0:sections[0].index(":")-1]
		else:
			self.title = title
		self.string = '\n'.join(sections)
		self.string = "%s: %s" % (self.title, self.string)

class OptionParser:
	esc_seq="\x1b["
	end=("%s39;49;00m" % (esc_seq))
	red=("%s31;01m" % (esc_seq))
	green=("%s32;01m" % (esc_seq))
	yellow=("%s33;01m" % (esc_seq))
	blue=("%s34;01m" % (esc_seq))
	magenta=("%s35;01m" % (esc_seq))
	cyan=("%s36;01m" % (esc_seq))
	bold=("\033[1m")
	def __init__(self, programName="default", desc="Generic Desciption", createHelp=True):
		self.options = []
		self.programName = programName
		self.desc = desc
	def setDescription(self, desc):
		self.desc = desc
	def setProgramName(self, name):
		self.programName = name
	def createHelp(self):
		if len(self.options) == 0:
			help_option = Option("-h", _desc="Display help page and exit")
			self.add_existing_option(help_option)
		self.title_cat = HelpCat("%s%s:%s %s" % (self.bold, self.programName, self.end, self.desc)).string
		self.usage_cat = HelpCat("Usage:\n   %s%s%s %s %s" % (self.cyan, self.programName, self.end, HelpArgument("options", "green"), HelpArgument("arguments", "cyan"))).string
		short_options = []
		optionSections = []
		for x in self.options:
			short_options.append(x.short.replace("-", ""))
			optionSections.append(SectionFromOption(x).string)
		short_options = ''.join(short_options)
		short_options = HelpArgument(short_options, "green", open_char="-[", close_char="]")
		optionSections.insert(0, short_options)
		self.option_cat = HelpCatFromSections("Options", optionSections).string
		self.help = "%s\n%s\n%s" % (self.title_cat, self.usage_cat, self.option_cat)
	def add_existing_option(self, option):
		if isinstance(option, PyOption.Option.Option):
			raise OptionError("Argument 'option' must be an Option/OptionWithArgs instance")
		self.options.append(option)
	def add_option(self, _short, _full=None, _desc=None, numofargs=0, argtype=str, default=False):
		if numofargs > 0:
			option = OptionWithArgs(_short, _full, _desc, numofargs, argtype, default)
		else:
			option = Option(_short, _full=None, _desc=None, default=False)
		self.add_existing_option(option)
